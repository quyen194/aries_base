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
      current_row_(nullptr),
      lengths_(nullptr),
      num_fields_(0),
      fields_(nullptr) {
  if (result_) {
    num_fields_ = mysql_num_fields(result_);
    fields_ = mysql_fetch_fields(result_);
  }
}
// -----------------------------------------------------------------------------

MySQLResult::~MySQLResult() {
  if (result_) {
    mysql_free_result(result_);
  }
}
// -----------------------------------------------------------------------------

bool MySQLResult::Next() {
  if (!result_) {
    return false;
  }

  current_row_ = mysql_fetch_row(result_);
  if (current_row_) {
    lengths_ = mysql_fetch_lengths(result_);
    return true;
  }

  return false;
}
// -----------------------------------------------------------------------------

int MySQLResult::GetInt(int col) {
  if (!current_row_ || col >= num_fields_ || !current_row_[col]) {
    return 0;
  }
  return std::atoi(current_row_[col]);
}
// -----------------------------------------------------------------------------

int64_t MySQLResult::GetInt64(int col) {
  if (!current_row_ || col >= num_fields_ || !current_row_[col]) {
    return 0;
  }
  return std::stoll(current_row_[col]);
}
// -----------------------------------------------------------------------------

double MySQLResult::GetDouble(int col) {
  if (!current_row_ || col >= num_fields_ || !current_row_[col]) {
    return 0.0;
  }
  return std::atof(current_row_[col]);
}
// -----------------------------------------------------------------------------

std::string MySQLResult::GetString(int col) {
  if (!current_row_ || col >= num_fields_ || !current_row_[col]) {
    return "";
  }
  return std::string(current_row_[col], lengths_[col]);
}
// -----------------------------------------------------------------------------

std::vector<uint8_t> MySQLResult::GetBlob(int col) {
  if (!current_row_ || col >= num_fields_ || !current_row_[col]) {
    return {};
  }

  const uint8_t* data = reinterpret_cast<const uint8_t*>(current_row_[col]);
  return std::vector<uint8_t>(data, data + lengths_[col]);
}
// -----------------------------------------------------------------------------

bool MySQLResult::IsNull(int col) {
  if (!current_row_ || col >= num_fields_) {
    return true;
  }
  return current_row_[col] == nullptr;
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
  for (int i = 0; i < num_fields_; ++i) {
    if (GetColumnName(i) == columnName) {
      return i;
    }
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

// -----------------------------------------------------------------------------
} // namespace mysql
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace database
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------
