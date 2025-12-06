/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/06 22:34
  filename:  aries_base/database/sqlite/sqlite_database.cpp

  purpose:   SQLite database implementation
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cstring>
#include <stdexcept>

#include "aries_base/database/sqlite/sqlite_database.hpp"
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

SQLiteDatabase::SQLiteDatabase() : db_(nullptr), connected_(false) {}
// -----------------------------------------------------------------------------

SQLiteDatabase::~SQLiteDatabase() {
  Disconnect();
}
// -----------------------------------------------------------------------------

bool SQLiteDatabase::Connect(const std::string& connectionString) {
  if (connected_) {
    Disconnect();
  }

  int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  int rc = sqlite3_open_v2(connectionString.c_str(), &db_, flags, nullptr);

  if (rc != SQLITE_OK) {
    last_error_ = sqlite3_errmsg(db_);
    sqlite3_close(db_);
    db_ = nullptr;
    connected_ = false;
    return false;
  }

  connected_ = true;
  last_error_.clear();
  return true;
}
// -----------------------------------------------------------------------------

void SQLiteDatabase::Disconnect() {
  if (db_) {
    sqlite3_close(db_);
    db_ = nullptr;
  }
  connected_ = false;
}
// -----------------------------------------------------------------------------

bool SQLiteDatabase::IsConnected() const {
  return connected_;
}
// -----------------------------------------------------------------------------

bool SQLiteDatabase::Begin() {
  auto result = Execute("BEGIN TRANSACTION");
  return result != nullptr;
}
// -----------------------------------------------------------------------------

bool SQLiteDatabase::Commit() {
  auto result = Execute("COMMIT");
  return result != nullptr;
}
// -----------------------------------------------------------------------------

bool SQLiteDatabase::Rollback() {
  auto result = Execute("ROLLBACK");
  return result != nullptr;
}
// -----------------------------------------------------------------------------

std::unique_ptr<ResultSets> SQLiteDatabase::Execute(const std::string& sql) {
  if (!connected_) {
    last_error_ = "Not connected to database";
    return nullptr;
  }

  sqlite3_stmt* stmt = nullptr;
  int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);

  if (rc != SQLITE_OK) {
    last_error_ = sqlite3_errmsg(db_);
    return nullptr;
  }

  rc = sqlite3_step(stmt);

  if (rc == SQLITE_DONE) {
    // No results (INSERT, UPDATE, DELETE, etc.)
    sqlite3_finalize(stmt);
    last_error_.clear();
    return std::make_unique<SQLiteResult>(nullptr);
  } else if (rc == SQLITE_ROW) {
    // Has results (SELECT)
    last_error_.clear();
    return std::make_unique<SQLiteResult>(stmt);
  } else {
    // Error
    last_error_ = sqlite3_errmsg(db_);
    sqlite3_finalize(stmt);
    return nullptr;
  }
}
// -----------------------------------------------------------------------------

std::unique_ptr<Statement> SQLiteDatabase::Prepare(const std::string& sql) {
  if (!connected_) {
    last_error_ = "Not connected to database";
    return nullptr;
  }

  try {
    return std::make_unique<SQLiteStatement>(db_, sql);
  } catch (const std::exception& e) {
    last_error_ = e.what();
    return nullptr;
  }
}
// -----------------------------------------------------------------------------

std::string SQLiteDatabase::GetLastError() const {
  return last_error_;
}
// -----------------------------------------------------------------------------

int64_t SQLiteDatabase::GetLastInsertId() const {
  if (!connected_) {
    return 0;
  }
  return sqlite3_last_insert_rowid(db_);
}
// -----------------------------------------------------------------------------

int64_t SQLiteDatabase::GetAffectedRows() const {
  if (!connected_) {
    return 0;
  }
  return sqlite3_changes(db_);
}
// -----------------------------------------------------------------------------

std::string SQLiteDatabase::ExecutePragma(const std::string& pragma) {
  std::string sql = "PRAGMA " + pragma;
  auto result = Execute(sql);

  if (result && result->Next()) {
    return result->GetString(0);
  }

  return "";
}
// -----------------------------------------------------------------------------

void SQLiteDatabase::SetBusyTimeout(int milliseconds) {
  if (connected_) {
    sqlite3_busy_timeout(db_, milliseconds);
  }
}
// -----------------------------------------------------------------------------

std::string SQLiteDatabase::GetLibraryVersion() {
  return sqlite3_libversion();
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
