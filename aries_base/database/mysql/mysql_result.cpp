/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 06:06
  filename:  aries_base/database/mysql/mysql_result.cpp

  purpose:   MySQL result set implementation
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cstring>

#include "aries_base/database/mysql/mysql_result.hpp"
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

MySQLResult::MySQLResult(MYSQL_RES* result)
    : result_(result),
      stmt_(nullptr),
      current_row_(nullptr),
      num_fields_(0),
      fields_(nullptr) {
  if (result_) {
    num_fields_ = mysql_num_fields(result_);
    fields_ = mysql_fetch_fields(result_);
    for (int i = 0; i < num_fields_; ++i) {
      column_map_[fields_[i].name] = i;
    }
  }
}
// -----------------------------------------------------------------------------

MySQLResult::MySQLResult(MYSQL_STMT* stmt)
    : result_(nullptr), stmt_(stmt), num_fields_(0) {
  MYSQL_RES* result_metadata = mysql_stmt_result_metadata(stmt_);
  if (result_metadata) {
    num_fields_ = mysql_num_fields(result_metadata);
    fields_ = mysql_fetch_fields(result_metadata);
    for (int i = 0; i < num_fields_; ++i) {
      column_map_[fields_[i].name] = i;
    }
    mysql_free_result(result_metadata);
    BindResultColumns();
  }
}
// -----------------------------------------------------------------------------

MySQLResult::~MySQLResult() {
  if (result_) {
    mysql_free_result(result_);
  }
  if (stmt_) {
    mysql_stmt_free_result(stmt_);
  }
}
// -----------------------------------------------------------------------------

bool MySQLResult::Next() {
  if (stmt_) {
    int status = mysql_stmt_fetch(stmt_);
    if (status == 0) {
      return true;
    } else if (status == MYSQL_NO_DATA) {
      return false;
    } else {
      // Handle error
      return false;
    }
  } else if (result_) {
    current_row_ = mysql_fetch_row(result_);
    if (current_row_) {
      unsigned long* lengths = mysql_fetch_lengths(result_);
      lengths_.resize(num_fields_);
      for (int i = 0; i < num_fields_; ++i) {
        lengths_[i] = lengths[i];
      }
      return true;
    }
    return false;
  }
  return false;
}
// -----------------------------------------------------------------------------

int MySQLResult::GetInt(int col) {
  if (IsNull(col)) {
    return 0;
  }
  if (stmt_) {
    return *reinterpret_cast<int*>(buffers_[col].data());
  } else if (result_) {
    return std::atoi(current_row_[col]);
  }
  return 0;
}
// -----------------------------------------------------------------------------

int64_t MySQLResult::GetInt64(int col) {
  if (IsNull(col)) {
    return 0;
  }
  if (stmt_) {
    return *reinterpret_cast<int64_t*>(buffers_[col].data());
  } else if (result_) {
    return std::stoll(current_row_[col]);
  }
  return 0;
}
// -----------------------------------------------------------------------------

double MySQLResult::GetDouble(int col) {
  if (IsNull(col)) {
    return 0.0;
  }
  if (stmt_) {
    return *reinterpret_cast<double*>(buffers_[col].data());
  } else if (result_) {
    return std::atof(current_row_[col]);
  }
  return 0.0;
}
// -----------------------------------------------------------------------------

std::string MySQLResult::GetString(int col) {
  if (IsNull(col)) {
    return "";
  }
  if (stmt_) {
    return std::string(buffers_[col].data(), lengths_[col]);
  } else if (result_) {
    return std::string(current_row_[col], lengths_[col]);
  }
  return "";
}
// -----------------------------------------------------------------------------

std::vector<uint8_t> MySQLResult::GetBlob(int col) {
  if (IsNull(col)) {
    return {};
  }
  if (stmt_) {
    const uint8_t* data = reinterpret_cast<const uint8_t*>(buffers_[col].data());
    return std::vector<uint8_t>(data, data + lengths_[col]);
  } else if (result_) {
    const uint8_t* data = reinterpret_cast<const uint8_t*>(current_row_[col]);
    return std::vector<uint8_t>(data, data + lengths_[col]);
  }
  return {};
}
// -----------------------------------------------------------------------------

bool MySQLResult::IsNull(int col) {
  if (col < 0 || col >= num_fields_) {
    return true;
  }
  if (stmt_) {
    return null_indicators_[col];
  } else if (result_) {
    return current_row_[col] == nullptr;
  }
  return true; // Should not happen
}
// -----------------------------------------------------------------------------

int MySQLResult::GetColumnCount() const {
  return num_fields_;
}
// -----------------------------------------------------------------------------

std::string MySQLResult::GetColumnName(int col) const {
  if (!fields_ || col >= num_fields_) {
    return "";
  }
  return fields_[col].name;
}
// -----------------------------------------------------------------------------

int MySQLResult::GetColumnIndex(const std::string& columnName) const {
  auto it = column_map_.find(columnName);
  if (it != column_map_.end()) {
    return it->second;
  }
  return -1;
}
// -----------------------------------------------------------------------------

int MySQLResult::GetInt(const std::string& columnName) {
  return GetInt(GetColumnIndex(columnName));
}
// -----------------------------------------------------------------------------

int64_t MySQLResult::GetInt64(const std::string& columnName) {
  return GetInt64(GetColumnIndex(columnName));
}
// -----------------------------------------------------------------------------

double MySQLResult::GetDouble(const std::string& columnName) {
  return GetDouble(GetColumnIndex(columnName));
}
// -----------------------------------------------------------------------------

std::string MySQLResult::GetString(const std::string& columnName) {
  return GetString(GetColumnIndex(columnName));
}
// -----------------------------------------------------------------------------

bool MySQLResult::IsNull(const std::string& columnName) {
  return IsNull(GetColumnIndex(columnName));
}
// -----------------------------------------------------------------------------

void MySQLResult::BindResultColumns() {
  binds_.resize(num_fields_);
  buffers_.resize(num_fields_);
  null_indicators_.resize(num_fields_);
  lengths_.resize(num_fields_);

  for (int i = 0; i < num_fields_; ++i) {
    MYSQL_FIELD* field = &fields_[i];
    size_t buffer_size = 0;
    switch (field->type) {
      case MYSQL_TYPE_TINY:
        buffer_size = sizeof(signed char);
        break;
      case MYSQL_TYPE_SHORT:
        buffer_size = sizeof(short);
        break;
      case MYSQL_TYPE_LONG:
      case MYSQL_TYPE_INT24:
        buffer_size = sizeof(int);
        break;
      case MYSQL_TYPE_LONGLONG:
        buffer_size = sizeof(long long);
        break;
      case MYSQL_TYPE_FLOAT:
        buffer_size = sizeof(float);
        break;
      case MYSQL_TYPE_DOUBLE:
        buffer_size = sizeof(double);
        break;
      case MYSQL_TYPE_VAR_STRING:
      case MYSQL_TYPE_STRING:
      case MYSQL_TYPE_BLOB:
      case MYSQL_TYPE_TINY_BLOB:
      case MYSQL_TYPE_MEDIUM_BLOB:
      case MYSQL_TYPE_LONG_BLOB:
        buffer_size = field->max_length > 0 ? field->max_length : 255; // Default for cases where max_length is 0
        break;
      default:
        buffer_size = 255; // Generic default
        break;
    }
    buffers_[i].resize(buffer_size);
    memset(&binds_[i], 0, sizeof(MYSQL_BIND));
    binds_[i].buffer_type = field->type;
    binds_[i].buffer = buffers_[i].data();
    binds_[i].buffer_length = buffers_[i].size();
    binds_[i].is_null = reinterpret_cast<bool*>(&null_indicators_[i]);
    binds_[i].length = &lengths_[i];
  }

  if (mysql_stmt_bind_result(stmt_, binds_.data()) != 0) {
    // Handle error - ideally, log this or throw an exception
  }
}

// -----------------------------------------------------------------------------
} // namespace mysql
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace database
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------
