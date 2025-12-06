/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/06 22:01
  filename:  aries_base/database/sqlite/sqlite_database.hpp

  purpose:   SQLite database implementation
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_DATABASE_SQLITE_SQLITE_DATABASE_HPP
#define ARIES_BASE_DATABASE_SQLITE_SQLITE_DATABASE_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <cstdint>
#include <string>
#include <memory>

#include <sqlite3.h>

#include "aries_base/database/interfaces/database.hpp"
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
/**
 * @brief SQLite database implementation
 */
class SQLiteDatabase : public Database {
 public:
  SQLiteDatabase();
  ~SQLiteDatabase() override;

  // Database interface
  bool Connect(const std::string& connectionString) override;
  void Disconnect() override;
  bool IsConnected() const override;
  bool Begin() override;
  bool Commit() override;
  bool Rollback() override;
  std::unique_ptr<ResultSets> Execute(const std::string& sql) override;
  std::unique_ptr<Statement> Prepare(const std::string& sql) override;
  std::string GetLastError() const override;
  int64_t GetLastInsertId() const override;
  int64_t GetAffectedRows() const override;

  // SQLite-specific extensions

  /**
   * @brief Execute a PRAGMA statement
   * @param pragma PRAGMA statement (e.g., "journal_mode=WAL")
   * @return Result string
   */
  std::string ExecutePragma(const std::string& pragma);

  /**
   * @brief Set busy timeout for locked databases
   * @param milliseconds Timeout in milliseconds
   */
  void SetBusyTimeout(int milliseconds);

  /**
   * @brief Get the SQLite library version
   * @return Version string
   */
  static std::string GetLibraryVersion();

  // Internal access (for SQLiteStatement and SQLiteResult)
  sqlite3* GetHandle() { return db_; }

 private:
  sqlite3* db_;
  std::string last_error_;
  bool connected_;
};
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

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_DATABASE_SQLITE_SQLITE_DATABASE_HPP
// -----------------------------------------------------------------------------
