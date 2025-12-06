/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 06:32
  filename:  aries_base/database/postgres/postgres_result.cpp

  purpose:   PostgreSQL result set implementation
*********************************************************************/


// -----------------------------------------------------------------------------
#include "aries_base/database/postgres/postgres_result.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace database {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace postgres {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

PostgresResult::PostgresResult(PGresult* result)
    : result_(result), current_row_(-1), num_rows_(0), num_fields_(0) {
  if (result_) {
    num_rows_ = PQntuples(result_);
    num_fields_ = PQnfields(result_);
  }
}
// -----------------------------------------------------------------------------

PostgresResult::~PostgresResult() {
  if (result_) {
    PQclear(result_);
  }
}
// -----------------------------------------------------------------------------

bool PostgresResult::Next() {
  if (!result_ || current_row_ >= num_rows_ - 1) {
    return false;
  }

  ++current_row_;
  return current_row_ < num_rows_;
}
// -----------------------------------------------------------------------------

int PostgresResult::GetInt(int col) {
  if (!result_ || current_row_ < 0 || col >= num_fields_) {
    return 0;
  }

  if (PQgetisnull(result_, current_row_, col)) {
    return 0;
  }

  return std::atoi(PQgetvalue(result_, current_row_, col));
}
// -----------------------------------------------------------------------------

int64_t PostgresResult::GetInt64(int col) {
  if (!result_ || current_row_ < 0 || col >= num_fields_) {
    return 0;
  }

  if (PQgetisnull(result_, current_row_, col)) {
    return 0;
  }

  return std::stoll(PQgetvalue(result_, current_row_, col));
}
// -----------------------------------------------------------------------------

double PostgresResult::GetDouble(int col) {
  if (!result_ || current_row_ < 0 || col >= num_fields_) {
    return 0.0;
  }

  if (PQgetisnull(result_, current_row_, col)) {
    return 0.0;
  }

  return std::atof(PQgetvalue(result_, current_row_, col));
}
// -----------------------------------------------------------------------------

std::string PostgresResult::GetString(int col) {
  if (!result_ || current_row_ < 0 || col >= num_fields_) {
    return "";
  }

  if (PQgetisnull(result_, current_row_, col)) {
    return "";
  }

  return PQgetvalue(result_, current_row_, col);
}
// -----------------------------------------------------------------------------

std::vector<uint8_t> PostgresResult::GetBlob(int col) {
  if (!result_ || current_row_ < 0 || col >= num_fields_) {
    return {};
  }

  if (PQgetisnull(result_, current_row_, col)) {
    return {};
  }

  int length = PQgetlength(result_, current_row_, col);
  const char* data = PQgetvalue(result_, current_row_, col);

  return std::vector<uint8_t>(reinterpret_cast<const uint8_t*>(data),
                              reinterpret_cast<const uint8_t*>(data) + length);
}
// -----------------------------------------------------------------------------

bool PostgresResult::IsNull(int col) {
  if (!result_ || current_row_ < 0 || col >= num_fields_) {
    return true;
  }

  return PQgetisnull(result_, current_row_, col) != 0;
}
// -----------------------------------------------------------------------------

int PostgresResult::GetColumnCount() const {
  return num_fields_;
}
// -----------------------------------------------------------------------------

std::string PostgresResult::GetColumnName(int col) const {
  if (!result_ || col >= num_fields_) {
    return "";
  }

  return PQfname(result_, col);
}
// -----------------------------------------------------------------------------

int PostgresResult::GetColumnIndex(const std::string& columnName) const {
  if (!result_) {
    return -1;
  }

  return PQfnumber(result_, columnName.c_str());
}
// -----------------------------------------------------------------------------

int PostgresResult::GetInt(const std::string& columnName) {
  return GetInt(GetColumnIndex(columnName));
}
// -----------------------------------------------------------------------------

int64_t PostgresResult::GetInt64(const std::string& columnName) {
  return GetInt64(GetColumnIndex(columnName));
}
// -----------------------------------------------------------------------------

double PostgresResult::GetDouble(const std::string& columnName) {
  return GetDouble(GetColumnIndex(columnName));
}
// -----------------------------------------------------------------------------

std::string PostgresResult::GetString(const std::string& columnName) {
  return GetString(GetColumnIndex(columnName));
}
// -----------------------------------------------------------------------------

bool PostgresResult::IsNull(const std::string& columnName) {
  return IsNull(GetColumnIndex(columnName));
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace postgres
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace database
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------
