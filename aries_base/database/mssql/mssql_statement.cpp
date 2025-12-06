/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/06 23:00
  filename:  aries_base/database/mssql/mssql_statement.cpp

  purpose:   MSSQL prepared statement implementation
*********************************************************************/


// -----------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#endif  // _WIN32

#include <stdexcept>

#include <sql.h>
#include <sqlext.h>

#include "aries_base/database/mssql/mssql_result.hpp"
#include "aries_base/database/mssql/mssql_statement.hpp"
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

MSSQLStatement::MSSQLStatement(SQLHDBC hdbc, const std::string& sql)
    : hdbc_(hdbc), hstmt_(SQL_NULL_HSTMT), sql_(sql) {
  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc_, &hstmt_);

  if (!SQL_SUCCEEDED(ret)) {
    throw std::runtime_error("Failed to allocate statement handle");
  }

  ret = SQLPrepare(hstmt_, (SQLCHAR*)sql_.c_str(), SQL_NTS);

  if (!SQL_SUCCEEDED(ret)) {
    last_error_ = GetODBCError();
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt_);
    throw std::runtime_error("Failed to prepare statement: " + last_error_);
  }

  // Get parameter count
  SQLSMALLINT paramCount;
  SQLNumParams(hstmt_, &paramCount);
  bindings_.resize(paramCount);
}
// -----------------------------------------------------------------------------

MSSQLStatement::~MSSQLStatement() {
  if (hstmt_ != SQL_NULL_HSTMT) {
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt_);
  }
}
// -----------------------------------------------------------------------------

std::string MSSQLStatement::GetODBCError() {
  SQLCHAR sqlState[6];
  SQLCHAR message[SQL_MAX_MESSAGE_LENGTH];
  SQLINTEGER nativeError;
  SQLSMALLINT msgLen;

  std::string errorMsg;

  SQLRETURN ret = SQLGetDiagRec(SQL_HANDLE_STMT,
                                hstmt_,
                                1,
                                sqlState,
                                &nativeError,
                                message,
                                sizeof(message),
                                &msgLen);

  if (SQL_SUCCEEDED(ret)) {
    errorMsg = std::string(reinterpret_cast<char*>(message), msgLen);
  }

  return errorMsg;
}
// -----------------------------------------------------------------------------

void MSSQLStatement::BindInt(int index, int value) {
  if (index < 1 || index > static_cast<int>(bindings_.size())) {
    return;
  }

  BindData& data = bindings_[index - 1];
  data.type = BindData::Type::INT;
  data.intVal = value;
  data.indicator = 0;
}
// -----------------------------------------------------------------------------

void MSSQLStatement::BindInt64(int index, int64_t value) {
  if (index < 1 || index > static_cast<int>(bindings_.size())) {
    return;
  }

  BindData& data = bindings_[index - 1];
  data.type = BindData::Type::INT64;
  data.int64Val = value;
  data.indicator = 0;
}
// -----------------------------------------------------------------------------

void MSSQLStatement::BindDouble(int index, double value) {
  if (index < 1 || index > static_cast<int>(bindings_.size())) {
    return;
  }

  BindData& data = bindings_[index - 1];
  data.type = BindData::Type::DOUBLE;
  data.doubleVal = value;
  data.indicator = 0;
}
// -----------------------------------------------------------------------------

void MSSQLStatement::BindString(int index, const std::string& value) {
  if (index < 1 || index > static_cast<int>(bindings_.size())) {
    return;
  }

  BindData& data = bindings_[index - 1];
  data.type = BindData::Type::STRING;
  data.stringVal = value;
  data.indicator = SQL_NTS;
}
// -----------------------------------------------------------------------------

void MSSQLStatement::BindNull(int index) {
  if (index < 1 || index > static_cast<int>(bindings_.size())) {
    return;
  }

  BindData& data = bindings_[index - 1];
  data.type = BindData::Type::NULL_VAL;
  data.indicator = SQL_NULL_DATA;
}
// -----------------------------------------------------------------------------

void MSSQLStatement::BindBlob(int index, const void* data, size_t size) {
  if (index < 1 || index > static_cast<int>(bindings_.size())) {
    return;
  }

  BindData& bindData = bindings_[index - 1];
  bindData.type = BindData::Type::BLOB;
  bindData.blobVal.assign(static_cast<const uint8_t*>(data),
               static_cast<const uint8_t*>(data) + size);
  bindData.indicator = size;
}
// -----------------------------------------------------------------------------

bool MSSQLStatement::Execute() {
  // Bind all parameters
  for (size_t i = 0; i < bindings_.size(); ++i) {
    BindData& data = bindings_[i];
    SQLUSMALLINT paramNum = static_cast<SQLUSMALLINT>(i + 1);
    SQLRETURN ret;

    switch (data.type) {
      case BindData::Type::INT:
        ret = SQLBindParameter(hstmt_,
                               paramNum,
                               SQL_PARAM_INPUT,
                               SQL_C_SLONG,
                               SQL_INTEGER,
                               0,
                               0,
                               &data.intVal,
                               0,
                               &data.indicator);
        break;
      case BindData::Type::INT64:
        ret = SQLBindParameter(hstmt_,
                               paramNum,
                               SQL_PARAM_INPUT,
                               SQL_C_SBIGINT,
                               SQL_BIGINT,
                               0,
                               0,
                               &data.int64Val,
                               0,
                               &data.indicator);
        break;
      case BindData::Type::DOUBLE:
        ret = SQLBindParameter(hstmt_,
                               paramNum,
                               SQL_PARAM_INPUT,
                               SQL_C_DOUBLE,
                               SQL_DOUBLE,
                               0,
                               0,
                               &data.doubleVal,
                               0,
                               &data.indicator);
        break;
      case BindData::Type::STRING:
        ret = SQLBindParameter(hstmt_,
                               paramNum,
                               SQL_PARAM_INPUT,
                               SQL_C_CHAR,
                               SQL_VARCHAR,
                               data.stringVal.length(),
                               0,
                               (SQLPOINTER) data.stringVal.c_str(),
                               data.stringVal.length(),
                               &data.indicator);
        break;
      case BindData::Type::BLOB:
        ret = SQLBindParameter(hstmt_,
                               paramNum,
                               SQL_PARAM_INPUT,
                               SQL_C_BINARY,
                               SQL_VARBINARY,
                               data.blobVal.size(),
                               0,
                               data.blobVal.data(),
                               data.blobVal.size(),
                               &data.indicator);
        break;
      case BindData::Type::NULL_VAL:
        ret = SQLBindParameter(hstmt_,
                               paramNum,
                               SQL_PARAM_INPUT,
                               SQL_C_CHAR,
                               SQL_VARCHAR,
                               0,
                               0,
                               nullptr,
                               0,
                               &data.indicator);
        break;
    }

    if (!SQL_SUCCEEDED(ret)) {
      last_error_ = GetODBCError();
      return false;
    }
  }

  SQLRETURN ret = SQLExecute(hstmt_);

  if (!SQL_SUCCEEDED(ret)) {
    last_error_ = GetODBCError();
    return false;
  }

  last_error_.clear();
  return true;
}
// -----------------------------------------------------------------------------

std::unique_ptr<ResultSets> MSSQLStatement::Query() {
  if (!Execute()) {
    return nullptr;
  }

  return std::make_unique<MSSQLResult>(hstmt_, false);
}
// -----------------------------------------------------------------------------

void MSSQLStatement::Reset() {
  SQLFreeStmt(hstmt_, SQL_RESET_PARAMS);
  SQLCloseCursor(hstmt_);
  last_error_.clear();
}
// -----------------------------------------------------------------------------

void MSSQLStatement::ClearBindings() {
  SQLFreeStmt(hstmt_, SQL_RESET_PARAMS);
  for (auto& binding : bindings_) {
    binding.indicator = 0;
  }
  last_error_.clear();
}
// -----------------------------------------------------------------------------

std::string MSSQLStatement::GetLastError() const {
  return last_error_;
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
