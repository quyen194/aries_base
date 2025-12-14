/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/06 22:41
  filename:  aries_base/database/sqlite/sqlite_result.hpp

  purpose:   SQLite result set implementation
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_DATABASE_SQLITE_SQLITE_RESULT_HPP
#define ARIES_BASE_DATABASE_SQLITE_SQLITE_RESULT_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <cstdint>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "aries_base/database/interfaces/result_sets.hpp"
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
 * @brief SQLite result set implementation
 */
class SQLiteResult : public ResultSets {
 public:
  SQLiteResult(sqlite3_stmt* stmt, bool owns_statement = false);
  ~SQLiteResult() override;

  // ResultSets interface
  bool Next() override;
  int GetInt(int col) override;
  int64_t GetInt64(int col) override;
  double GetDouble(int col) override;
  std::string GetString(int col) override;
  std::vector<uint8_t> GetBlob(int col) override;
  bool IsNull(int col) override;
  int GetColumnCount() const override;
  std::string GetColumnName(int col) const override;
  int GetInt(const std::string& columnName) override;
  int64_t GetInt64(const std::string& columnName) override;
  double GetDouble(const std::string& columnName) override;
  std::string GetString(const std::string& columnName) override;
  bool IsNull(const std::string& columnName) override;

 private:
  int GetColumnIndex(const std::string& columnName) const;

 private:
  sqlite3_stmt* stmt_;
  bool has_row_;
  bool owns_statement_;
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
#endif  // ARIES_BASE_DATABASE_SQLITE_SQLITE_RESULT_HPP
// -----------------------------------------------------------------------------
