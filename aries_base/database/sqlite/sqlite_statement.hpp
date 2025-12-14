/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/06 22:37
  filename:  aries_base/database/sqlite/sqlite_statement.hpp

  purpose:   SQLite prepared statement implementation
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_DATABASE_SQLITE_SQLITE_STATEMENT_HPP
#define ARIES_BASE_DATABASE_SQLITE_SQLITE_STATEMENT_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <cstdint>
#include <memory>
#include <string>

#include <sqlite3.h>

#include "aries_base/database/interfaces/statement.hpp"
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
/**
 * @brief SQLite prepared statement implementation
 */
class SQLiteStatement : public Statement {
 public:
  SQLiteStatement(sqlite3* db, const std::string& sql);
  ~SQLiteStatement() override;

  // Statement interface
  void BindInt(int index, int value) override;
  void BindInt64(int index, int64_t value) override;
  void BindDouble(int index, double value) override;
  void BindString(int index, const std::string& value) override;
  void BindNull(int index) override;
  void BindBlob(int index, const void* data, size_t size) override;
  bool Execute() override;
  std::unique_ptr<ResultSets> Query() override;
  void Reset() override;
  void ClearBindings() override;
  void Close() override;
  std::string GetLastError() const override;

 private:
  sqlite3* db_;
  sqlite3_stmt* stmt_;
  std::string sql_;
  std::string last_error_;
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
#endif  // ARIES_BASE_DATABASE_SQLITE_SQLITE_STATEMENT_HPP
// -----------------------------------------------------------------------------
