/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/06 22:45
  filename:  aries_base/database/sqlite/sqlite_result.cpp

  purpose:   SQLite result set implementation
*********************************************************************/


// -----------------------------------------------------------------------------
#include "aries_base/database/sqlite/sqlite_result.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace database {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace sqlite {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

SQLiteResult::SQLiteResult(sqlite3_stmt* stmt)
    : stmt_(stmt), has_row_(false), owns_statement_(stmt != nullptr) {}
// -----------------------------------------------------------------------------

SQLiteResult::~SQLiteResult() {
  // Note: We don't finalize the statement here because SQLiteStatement owns it
  // However, if this was created from Execute(), the statement was already handled
}
// -----------------------------------------------------------------------------

bool SQLiteResult::Next() {
  if (!stmt_) {
    has_row_ = false;
    return false;
  }

  int rc = sqlite3_step(stmt_);

  if (rc == SQLITE_ROW) {
    has_row_ = true;
    return true;
  } else if (rc == SQLITE_DONE) {
    has_row_ = false;
    if (owns_statement_) {
      sqlite3_finalize(stmt_);
      stmt_ = nullptr;
    }
    return false;
  } else {
    has_row_ = false;
    return false;
  }
}
// -----------------------------------------------------------------------------

int SQLiteResult::GetInt(int col) {
  if (!has_row_) {
    return 0;
  }
  return sqlite3_column_int(stmt_, col);
}
// -----------------------------------------------------------------------------

int64_t SQLiteResult::GetInt64(int col) {
  if (!has_row_) {
    return 0;
  }
  return sqlite3_column_int64(stmt_, col);
}
// -----------------------------------------------------------------------------

double SQLiteResult::GetDouble(int col) {
  if (!has_row_) {
    return 0.0;
  }
  return sqlite3_column_double(stmt_, col);
}
// -----------------------------------------------------------------------------

std::string SQLiteResult::GetString(int col) {
  if (!has_row_) {
    return "";
  }

  const unsigned char* text = sqlite3_column_text(stmt_, col);
  if (text) {
    return std::string(reinterpret_cast<const char*>(text));
  }
  return "";
}
// -----------------------------------------------------------------------------

std::vector<uint8_t> SQLiteResult::GetBlob(int col) {
  if (!has_row_) {
    return {};
  }

  const void* blob = sqlite3_column_blob(stmt_, col);
  int size = sqlite3_column_bytes(stmt_, col);

  if (blob && size > 0) {
    const uint8_t* data = static_cast<const uint8_t*>(blob);
    return std::vector<uint8_t>(data, data + size);
  }

  return {};
}
// -----------------------------------------------------------------------------

bool SQLiteResult::IsNull(int col) {
  if (!has_row_) {
    return true;
  }
  return sqlite3_column_type(stmt_, col) == SQLITE_NULL;
}
// -----------------------------------------------------------------------------

int SQLiteResult::GetColumnCount() const {
  if (!stmt_) {
    return 0;
  }
  return sqlite3_column_count(stmt_);
}
// -----------------------------------------------------------------------------

std::string SQLiteResult::GetColumnName(int col) const {
  if (!stmt_) {
    return "";
  }

  const char* name = sqlite3_column_name(stmt_, col);
  return name ? name : "";
}
// -----------------------------------------------------------------------------

int SQLiteResult::GetColumnIndex(const std::string& columnName) const {
  int count = GetColumnCount();
  for (int i = 0; i < count; ++i) {
    if (GetColumnName(i) == columnName) {
      return i;
    }
  }
  return -1;
}
// -----------------------------------------------------------------------------

int SQLiteResult::GetInt(const std::string& columnName) {
  return GetInt(GetColumnIndex(columnName));
}
// -----------------------------------------------------------------------------

int64_t SQLiteResult::GetInt64(const std::string& columnName) {
  return GetInt64(GetColumnIndex(columnName));
}
// -----------------------------------------------------------------------------

double SQLiteResult::GetDouble(const std::string& columnName) {
  return GetDouble(GetColumnIndex(columnName));
}
// -----------------------------------------------------------------------------

std::string SQLiteResult::GetString(const std::string& columnName) {
  return GetString(GetColumnIndex(columnName));
}
// -----------------------------------------------------------------------------

bool SQLiteResult::IsNull(const std::string& columnName) {
  return IsNull(GetColumnIndex(columnName));
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace sqlite
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace database
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------
