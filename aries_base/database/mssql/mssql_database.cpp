/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 06:13
  filename:  aries_base/database/mssql/mssql_database.cpp

  purpose:   Microsoft SQL Server database implementation (via ODBC)
*********************************************************************/


// -----------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#endif  // _WIN32

#include <cstring>
#include <sstream>
#include <stdexcept>

#include "aries_base/database/mssql/mssql_database.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace database {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace mssql {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

MSSQLDatabase::MSSQLDatabase()
    : henv_(SQL_NULL_HENV),
      hdbc_(SQL_NULL_HDBC),
      connected_(false),
      last_affected_rows_(0) {
  // Allocate environment handle
  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv_);
  if (!SQL_SUCCEEDED(ret)) {
    throw std::runtime_error("Failed to allocate ODBC environment handle");
  }

  // Set ODBC version
  SQLSetEnvAttr(henv_, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

  // Allocate connection handle
  ret = SQLAllocHandle(SQL_HANDLE_DBC, henv_, &hdbc_);
  if (!SQL_SUCCEEDED(ret)) {
    SQLFreeHandle(SQL_HANDLE_ENV, henv_);
    throw std::runtime_error("Failed to allocate ODBC connection handle");
  }
}
// -----------------------------------------------------------------------------

MSSQLDatabase::~MSSQLDatabase() {
  Disconnect();

  if (hdbc_ != SQL_NULL_HDBC) {
    SQLFreeHandle(SQL_HANDLE_DBC, hdbc_);
  }

  if (henv_ != SQL_NULL_HENV) {
    SQLFreeHandle(SQL_HANDLE_ENV, henv_);
  }
}
// -----------------------------------------------------------------------------

std::string MSSQLDatabase::GetODBCError(SQLSMALLINT handleType,
                                        SQLHANDLE handle) {
  SQLCHAR sqlState[6];
  SQLCHAR message[SQL_MAX_MESSAGE_LENGTH];
  SQLINTEGER nativeError;
  SQLSMALLINT msgLen;

  std::string errorMsg;

  SQLRETURN ret = SQLGetDiagRec(handleType,
                                handle,
                                1,
                                sqlState,
                                &nativeError,
                                message,
                                sizeof(message),
                                &msgLen);

  if (SQL_SUCCEEDED(ret)) {
    errorMsg = std::string(reinterpret_cast<char*>(message), msgLen);
    errorMsg += " (SQLState: " + std::string(reinterpret_cast<char*>(sqlState)) + ")";
  }

  return errorMsg;
}
// -----------------------------------------------------------------------------

bool MSSQLDatabase::Connect(const std::string& connectionString) {
  if (connected_) {
    Disconnect();
  }

  SQLCHAR outConnStr[1024];
  SQLSMALLINT outConnStrLen;

  SQLRETURN ret = SQLDriverConnect(hdbc_,
                                   nullptr,
                                   (SQLCHAR*) connectionString.c_str(),
                                   SQL_NTS,
                                   outConnStr,
                                   sizeof(outConnStr),
                                   &outConnStrLen,
                                   SQL_DRIVER_NOPROMPT);

  if (!SQL_SUCCEEDED(ret)) {
    last_error_ = GetODBCError(SQL_HANDLE_DBC, hdbc_);
    connected_ = false;
    return false;
  }

  // Set auto-commit mode on by default
  SQLSetConnectAttr(
      hdbc_, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) SQL_AUTOCOMMIT_ON, 0);

  connected_ = true;
  last_error_.clear();
  return true;
}
// -----------------------------------------------------------------------------

void MSSQLDatabase::Disconnect() {
  if (connected_) {
    SQLDisconnect(hdbc_);
    connected_ = false;
  }
}
// -----------------------------------------------------------------------------

bool MSSQLDatabase::IsConnected() const {
  return connected_;
}
// -----------------------------------------------------------------------------

bool MSSQLDatabase::Begin() {
  if (!connected_) {
    return false;
  }

  // Turn off auto-commit to start transaction
  SQLRETURN ret = SQLSetConnectAttr(
      hdbc_, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) SQL_AUTOCOMMIT_OFF, 0);

  if (!SQL_SUCCEEDED(ret)) {
    last_error_ = GetODBCError(SQL_HANDLE_DBC, hdbc_);
    return false;
  }

  return true;
}
// -----------------------------------------------------------------------------

bool MSSQLDatabase::Commit() {
  if (!connected_) {
    return false;
  }

  SQLRETURN ret = SQLEndTran(SQL_HANDLE_DBC, hdbc_, SQL_COMMIT);

  if (!SQL_SUCCEEDED(ret)) {
    last_error_ = GetODBCError(SQL_HANDLE_DBC, hdbc_);
    return false;
  }

  // Turn auto-commit back on
  SQLSetConnectAttr(
      hdbc_, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) SQL_AUTOCOMMIT_ON, 0);

  return true;
}
// -----------------------------------------------------------------------------

bool MSSQLDatabase::Rollback() {
  if (!connected_) {
    return false;
  }

  SQLRETURN ret = SQLEndTran(SQL_HANDLE_DBC, hdbc_, SQL_ROLLBACK);

  if (!SQL_SUCCEEDED(ret)) {
    last_error_ = GetODBCError(SQL_HANDLE_DBC, hdbc_);
    return false;
  }

  // Turn auto-commit back on
  SQLSetConnectAttr(
      hdbc_, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER) SQL_AUTOCOMMIT_ON, 0);

  return true;
}
// -----------------------------------------------------------------------------

std::unique_ptr<ResultSets> MSSQLDatabase::Execute(const std::string& sql) {
  if (!connected_) {
    last_error_ = "Not connected to database";
    return nullptr;
  }

  SQLHSTMT hstmt;
  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc_, &hstmt);

  if (!SQL_SUCCEEDED(ret)) {
    last_error_ = GetODBCError(SQL_HANDLE_DBC, hdbc_);
    return nullptr;
  }

  ret = SQLExecDirect(hstmt, (SQLCHAR*)sql.c_str(), SQL_NTS);

  if (!SQL_SUCCEEDED(ret)) {
    last_error_ = GetODBCError(SQL_HANDLE_STMT, hstmt);
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    return nullptr;
  }

  // Get affected rows
  SQLLEN rowCount;
  SQLRowCount(hstmt, &rowCount);
  last_affected_rows_ = rowCount;

  last_error_.clear();
  return std::make_unique<MSSQLResult>(hstmt);
}
// -----------------------------------------------------------------------------

std::unique_ptr<Statement> MSSQLDatabase::Prepare(const std::string& sql) {
  if (!connected_) {
    last_error_ = "Not connected to database";
    return nullptr;
  }

  try {
    return std::make_unique<MSSQLStatement>(hdbc_, sql);
  } catch (const std::exception& e) {
    last_error_ = e.what();
    return nullptr;
  }
}
// -----------------------------------------------------------------------------

std::string MSSQLDatabase::GetLastError() const {
  return last_error_;
}
// -----------------------------------------------------------------------------

int64_t MSSQLDatabase::GetLastInsertId() const {
  if (!connected_) {
    return 0;
  }

  // Execute SELECT SCOPE_IDENTITY() to get last insert ID
  SQLHSTMT hstmt;
  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc_, &hstmt);

  if (!SQL_SUCCEEDED(ret)) {
    return 0;
  }

  ret = SQLExecDirect(hstmt, (SQLCHAR*)"SELECT SCOPE_IDENTITY()", SQL_NTS);

  if (!SQL_SUCCEEDED(ret)) {
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    return 0;
  }

  ret = SQLFetch(hstmt);
  if (SQL_SUCCEEDED(ret)) {
    SQLLEN indicator;
    int64_t id = 0;
    SQLGetData(hstmt, 1, SQL_C_SBIGINT, &id, sizeof(id), &indicator);
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    return id;
  }

  SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
  return 0;
}
// -----------------------------------------------------------------------------

int64_t MSSQLDatabase::GetAffectedRows() const {
  return last_affected_rows_;
}
// -----------------------------------------------------------------------------

std::unique_ptr<ResultSets> MSSQLDatabase::ExecuteStoredProcedure(
    const std::string& procName, const std::vector<std::string>& params) {
  std::string sql = "EXEC " + procName;

  if (!params.empty()) {
    sql += " ";
    for (size_t i = 0; i < params.size(); ++i) {
      if (i > 0) sql += ", ";
      sql += params[i];
    }
  }

  return Execute(sql);
}
// -----------------------------------------------------------------------------

std::string MSSQLDatabase::GetServerVersion() {
  auto result = Execute("SELECT @@VERSION");

  if (result && result->Next()) {
    return result->GetString(0);
  }

  return "";
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace mssql
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace database
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------
