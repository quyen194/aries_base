/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 06:09
  filename:  aries_base/database/mysql/mysql_statement.cpp

  purpose:   MySQL prepared statement implementation
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cstring>
#include <stdexcept>
#include <string>
#include <memory>
#include <map>
#include <vector>

#include <mysql/mysql.h>

#include "aries_base/database/mysql/mysql_statement.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace database {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace mysql {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

MySQLStatement::MySQLStatement(MYSQL* mysql, const std::string& sql)
    : mysql_(mysql), stmt_(nullptr), sql_(sql) {
  stmt_ = mysql_stmt_init(mysql_);
  if (!stmt_) {
    throw std::runtime_error("Failed to initialize statement");
  }

  if (mysql_stmt_prepare(stmt_, sql.c_str(), sql.length()) != 0) {
    last_error_ = mysql_stmt_error(stmt_);
    mysql_stmt_close(stmt_);
    throw std::runtime_error("Failed to prepare statement: " + last_error_);
  }

  int paramCount = mysql_stmt_param_count(stmt_);
  mysql_binds_.resize(paramCount);
  memset(mysql_binds_.data(), 0, sizeof(MYSQL_BIND) * paramCount);
}
// -----------------------------------------------------------------------------

MySQLStatement::~MySQLStatement() {
  if (stmt_) {
    mysql_stmt_close(stmt_);
  }
}
// -----------------------------------------------------------------------------

void MySQLStatement::BindInt(int index, int value) {
  BindData data;
  data.type = BindData::Type::INT;
  data.intVal = value;
  bindings_[index] = data;
}
// -----------------------------------------------------------------------------

void MySQLStatement::BindInt64(int index, int64_t value) {
  BindData data;
  data.type = BindData::Type::INT64;
  data.int64Val = value;
  bindings_[index] = data;
}
// -----------------------------------------------------------------------------

void MySQLStatement::BindDouble(int index, double value) {
  BindData data;
  data.type = BindData::Type::DOUBLE;
  data.doubleVal = value;
  bindings_[index] = data;
}
// -----------------------------------------------------------------------------

void MySQLStatement::BindString(int index, const std::string& value) {
  BindData data;
  data.type = BindData::Type::STRING;
  data.stringVal = value;
  bindings_[index] = data;
}
// -----------------------------------------------------------------------------

void MySQLStatement::BindNull(int index) {
  BindData data;
  data.type = BindData::Type::NULL_VAL;
  bindings_[index] = data;
}
// -----------------------------------------------------------------------------

void MySQLStatement::BindBlob(int index, const void* data, size_t size) {
  BindData bindData;
  bindData.type = BindData::Type::BLOB;
  bindData.blobVal.assign(static_cast<const uint8_t*>(data),
                          static_cast<const uint8_t*>(data) + size);
  bindings_[index] = bindData;
}
// -----------------------------------------------------------------------------

bool MySQLStatement::Execute() {
  // Prepare bindings
  for (auto& pair : bindings_) {
    int idx = pair.first - 1; // Convert 1-based to 0-based
    if (idx < 0 || idx >= static_cast<int>(mysql_binds_.size())) {
      continue;
    }

    BindData& data = pair.second;
    MYSQL_BIND& bind = mysql_binds_[idx];

    memset(&bind, 0, sizeof(MYSQL_BIND));

    switch (data.type) {
      case BindData::Type::INT:
        bind.buffer_type = MYSQL_TYPE_LONG;
        bind.buffer = &data.intVal;
        break;
      case BindData::Type::INT64:
        bind.buffer_type = MYSQL_TYPE_LONGLONG;
        bind.buffer = &data.int64Val;
        break;
      case BindData::Type::DOUBLE:
        bind.buffer_type = MYSQL_TYPE_DOUBLE;
        bind.buffer = &data.doubleVal;
        break;
      case BindData::Type::STRING:
        bind.buffer_type = MYSQL_TYPE_STRING;
        bind.buffer = const_cast<char*>(data.stringVal.c_str());
        bind.buffer_length = data.stringVal.length();
        break;
      case BindData::Type::BLOB:
        bind.buffer_type = MYSQL_TYPE_BLOB;
        bind.buffer = data.blobVal.data();
        bind.buffer_length = data.blobVal.size();
        break;
      case BindData::Type::NULL_VAL:
        bind.buffer_type = MYSQL_TYPE_NULL;
        break;
    }
  }

  if (!mysql_binds_.empty() &&
      mysql_stmt_bind_param(stmt_, mysql_binds_.data()) != 0) {
    last_error_ = mysql_stmt_error(stmt_);
    return false;
  }

  if (mysql_stmt_execute(stmt_) != 0) {
    last_error_ = mysql_stmt_error(stmt_);
    return false;
  }

  last_error_.clear();
  return true;
}
// -----------------------------------------------------------------------------

std::unique_ptr<ResultSets> MySQLStatement::Query() {
  if (!Execute()) {
    return nullptr;
  }

  MYSQL_RES* result = mysql_stmt_result_metadata(stmt_);
  if (!result) {
    return nullptr;
  }

  // For prepared statements, we'd need to bind result columns
  // For simplicity, we'll store the result
  mysql_stmt_store_result(stmt_);

  // Note: This is a simplified implementation
  // A full implementation would bind result columns properly
  mysql_free_result(result);

  return nullptr; // Simplified - would need proper result binding
}
// -----------------------------------------------------------------------------

void MySQLStatement::Reset() {
  mysql_stmt_reset(stmt_);
  last_error_.clear();
}
// -----------------------------------------------------------------------------

void MySQLStatement::ClearBindings() {
  bindings_.clear();
  memset(mysql_binds_.data(), 0, sizeof(MYSQL_BIND) * mysql_binds_.size());
  last_error_.clear();
}
// -----------------------------------------------------------------------------

std::string MySQLStatement::GetLastError() const {
  return last_error_;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace mysql
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace database
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------
