/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/06 22:47
  filename:  aries_base/database/sqlite/sqlite_statement.cpp

  purpose:   SQLite prepared statement implementation
*********************************************************************/


// -----------------------------------------------------------------------------
#include <stdexcept>
#include <cstring>

#include "aries_base/database/sqlite/sqlite_statement.hpp"
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

SQLiteStatement::SQLiteStatement(sqlite3* db, const std::string& sql)
    : db_(db), stmt_(nullptr), sql_(sql) {
  int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt_, nullptr);

  if (rc != SQLITE_OK) {
    last_error_ = sqlite3_errmsg(db_);
    throw std::runtime_error("Failed to prepare statement: " + last_error_);
  }
}
// -----------------------------------------------------------------------------

SQLiteStatement::~SQLiteStatement() {
  Close();
}
// -----------------------------------------------------------------------------

void SQLiteStatement::BindInt(int index, int value) {
  int rc = sqlite3_bind_int(stmt_, index, value);
  if (rc != SQLITE_OK) {
    last_error_ = sqlite3_errmsg(db_);
  }
}
// -----------------------------------------------------------------------------

void SQLiteStatement::BindInt64(int index, int64_t value) {
  int rc = sqlite3_bind_int64(stmt_, index, value);
  if (rc != SQLITE_OK) {
    last_error_ = sqlite3_errmsg(db_);
  }
}
// -----------------------------------------------------------------------------

void SQLiteStatement::BindDouble(int index, double value) {
  int rc = sqlite3_bind_double(stmt_, index, value);
  if (rc != SQLITE_OK) {
    last_error_ = sqlite3_errmsg(db_);
  }
}
// -----------------------------------------------------------------------------

void SQLiteStatement::BindString(int index, const std::string& value) {
  int rc = sqlite3_bind_text(stmt_, index, value.c_str(), -1, SQLITE_TRANSIENT);
  if (rc != SQLITE_OK) {
    last_error_ = sqlite3_errmsg(db_);
  }
}
// -----------------------------------------------------------------------------

void SQLiteStatement::BindNull(int index) {
  int rc = sqlite3_bind_null(stmt_, index);
  if (rc != SQLITE_OK) {
    last_error_ = sqlite3_errmsg(db_);
  }
}
// -----------------------------------------------------------------------------

void SQLiteStatement::BindBlob(int index, const void* data, size_t size) {
  int rc = sqlite3_bind_blob(
      stmt_, index, data, static_cast<int>(size), SQLITE_TRANSIENT);
  if (rc != SQLITE_OK) {
    last_error_ = sqlite3_errmsg(db_);
  }
}
// -----------------------------------------------------------------------------

bool SQLiteStatement::Execute() {
  int rc = sqlite3_step(stmt_);

  if (rc == SQLITE_DONE || rc == SQLITE_ROW) {
    sqlite3_reset(stmt_);
    last_error_.clear();
    return true;
  }

  last_error_ = sqlite3_errmsg(db_);
  sqlite3_reset(stmt_);
  return false;
}
// -----------------------------------------------------------------------------

std::unique_ptr<ResultSets> SQLiteStatement::Query() {
  int rc = sqlite3_step(stmt_);

  if (rc == SQLITE_ROW || rc == SQLITE_DONE) {
    last_error_.clear();
    // Note: SQLiteResult will take ownership of stepping through the statement
    // We reset position so it can start from the beginning
    sqlite3_reset(stmt_);
    return std::make_unique<SQLiteResult>(stmt_);
  }

  last_error_ = sqlite3_errmsg(db_);
  sqlite3_reset(stmt_);
  return nullptr;
}
// -----------------------------------------------------------------------------

void SQLiteStatement::Reset() {
  sqlite3_reset(stmt_);
  last_error_.clear();
}
// -----------------------------------------------------------------------------

void SQLiteStatement::ClearBindings() {
  sqlite3_clear_bindings(stmt_);
  last_error_.clear();
}
// -----------------------------------------------------------------------------

void SQLiteStatement::Close() {
  if (stmt_) {
    sqlite3_finalize(stmt_);
    stmt_ = nullptr;
  }
}
// -----------------------------------------------------------------------------

std::string SQLiteStatement::GetLastError() const {
  return last_error_;
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
