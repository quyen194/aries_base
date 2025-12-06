/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/06 22:59
  filename:  aries_base/database/mssql/mssql_result.cpp

  purpose:   MSSQL result set implementation
*********************************************************************/


// -----------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#endif  // _WIN32

#include "aries_base/database/mssql/mssql_result.hpp"
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

MSSQLResult::MSSQLResult(SQLHSTMT hstmt, bool ownsStatement)
    : hstmt_(hstmt),
      owns_statement_(ownsStatement),
      has_row_(false),
      num_cols_(0) {
  if (hstmt_) {
    SQLNumResultCols(hstmt_, &num_cols_);
  }
}
// -----------------------------------------------------------------------------

MSSQLResult::~MSSQLResult() {
  if (owns_statement_ && hstmt_ != SQL_NULL_HSTMT) {
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt_);
  }
}
// -----------------------------------------------------------------------------

std::string MSSQLResult::GetODBCError() {
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

bool MSSQLResult::Next() {
  if (!hstmt_) {
    return false;
  }

  SQLRETURN ret = SQLFetch(hstmt_);

  if (SQL_SUCCEEDED(ret)) {
    has_row_ = true;
    return true;
  }

  has_row_ = false;
  return false;
}
// -----------------------------------------------------------------------------

int MSSQLResult::GetInt(int col) {
  if (!has_row_ || col >= num_cols_) {
    return 0;
  }

  SQLINTEGER value;
  SQLLEN indicator;

  SQLRETURN ret = SQLGetData(
      hstmt_, col + 1, SQL_C_SLONG, &value, sizeof(value), &indicator);

  if (SQL_SUCCEEDED(ret) && indicator != SQL_NULL_DATA) {
    return value;
  }

  return 0;
}
// -----------------------------------------------------------------------------

int64_t MSSQLResult::GetInt64(int col) {
  if (!has_row_ || col >= num_cols_) {
    return 0;
  }

  SQLBIGINT value;
  SQLLEN indicator;

  SQLRETURN ret = SQLGetData(
      hstmt_, col + 1, SQL_C_SBIGINT, &value, sizeof(value), &indicator);

  if (SQL_SUCCEEDED(ret) && indicator != SQL_NULL_DATA) {
    return value;
  }

  return 0;
}
// -----------------------------------------------------------------------------

double MSSQLResult::GetDouble(int col) {
  if (!has_row_ || col >= num_cols_) {
    return 0.0;
  }

  SQLDOUBLE value;
  SQLLEN indicator;

  SQLRETURN ret = SQLGetData(
      hstmt_, col + 1, SQL_C_DOUBLE, &value, sizeof(value), &indicator);

  if (SQL_SUCCEEDED(ret) && indicator != SQL_NULL_DATA) {
    return value;
  }

  return 0.0;
}
// -----------------------------------------------------------------------------

std::string MSSQLResult::GetString(int col) {
  if (!has_row_ || col >= num_cols_) {
    return "";
  }

  char buffer[4096];
  SQLLEN indicator;

  SQLRETURN ret = SQLGetData(
      hstmt_, col + 1, SQL_C_CHAR, buffer, sizeof(buffer), &indicator);

  if (SQL_SUCCEEDED(ret) && indicator != SQL_NULL_DATA) {
    return std::string(buffer);
  }

  return "";
}
// -----------------------------------------------------------------------------

std::vector<uint8_t> MSSQLResult::GetBlob(int col) {
  if (!has_row_ || col >= num_cols_) {
    return {};
  }

  std::vector<uint8_t> result;
  uint8_t buffer[4096];
  SQLLEN indicator;

  SQLRETURN ret;
  while ((ret = SQLGetData(hstmt_,
                           col + 1,
                           SQL_C_BINARY,
                           buffer,
                           sizeof(buffer),
                           &indicator)) != SQL_NO_DATA) {
    if (SQL_SUCCEEDED(ret) && indicator != SQL_NULL_DATA) {
      size_t bytesToRead =
          (indicator > sizeof(buffer)) ? sizeof(buffer) : indicator;
      result.insert(result.end(), buffer, buffer + bytesToRead);
    }

    if (ret == SQL_SUCCESS) {
      break;
    }
  }

  return result;
}
// -----------------------------------------------------------------------------

bool MSSQLResult::IsNull(int col) {
  if (!has_row_ || col >= num_cols_) {
    return true;
  }

  char dummy;
  SQLLEN indicator;

  SQLRETURN ret =
      SQLGetData(hstmt_, col + 1, SQL_C_CHAR, &dummy, 0, &indicator);

  return indicator == SQL_NULL_DATA;
}
// -----------------------------------------------------------------------------

int MSSQLResult::GetColumnCount() const {
  return num_cols_;
}
// -----------------------------------------------------------------------------

std::string MSSQLResult::GetColumnName(int col) const {
  if (!hstmt_ || col >= num_cols_) {
    return "";
  }

  SQLCHAR columnName[256];
  SQLSMALLINT nameLen;
  SQLSMALLINT dataType;
  SQLULEN columnSize;
  SQLSMALLINT decimalDigits;
  SQLSMALLINT nullable;

  SQLRETURN ret = SQLDescribeCol(hstmt_,
                                 col + 1,
                                 columnName,
                                 sizeof(columnName),
                                 &nameLen,
                                 &dataType,
                                 &columnSize,
                                 &decimalDigits,
                                 &nullable);

  if (SQL_SUCCEEDED(ret)) {
    return std::string(reinterpret_cast<char*>(columnName));
  }

  return "";
}
// -----------------------------------------------------------------------------

int MSSQLResult::GetColumnIndex(const std::string& columnName) const {
  for (int i = 0; i < num_cols_; ++i) {
    if (GetColumnName(i) == columnName) {
      return i;
    }
  }
  return -1;
}
// -----------------------------------------------------------------------------

int MSSQLResult::GetInt(const std::string& columnName) {
  return GetInt(GetColumnIndex(columnName));
}
// -----------------------------------------------------------------------------

int64_t MSSQLResult::GetInt64(const std::string& columnName) {
  return GetInt64(GetColumnIndex(columnName));
}
// -----------------------------------------------------------------------------

double MSSQLResult::GetDouble(const std::string& columnName) {
  return GetDouble(GetColumnIndex(columnName));
}
// -----------------------------------------------------------------------------

std::string MSSQLResult::GetString(const std::string& columnName) {
  return GetString(GetColumnIndex(columnName));
}
// -----------------------------------------------------------------------------

bool MSSQLResult::IsNull(const std::string& columnName) {
  return IsNull(GetColumnIndex(columnName));
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
