/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 06:04
  filename:  aries_base/database/mysql/mysql_result.hpp

  purpose:   MySQL result set implementation
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_DATABASE_MYSQL_MYSQL_RESULT_HPP
#define ARIES_BASE_DATABASE_MYSQL_MYSQL_RESULT_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <cstdint>
#include <string>
#include <vector>

#include <mysql/mysql.h>

#include "aries_base/database/interfaces/result_sets.hpp"
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
/**
 * @brief MySQL result set implementation
 */
class MySQLResult : public ResultSets {
 public:
  MySQLResult(MYSQL_RES* result);
  ~MySQLResult() override;

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
  MYSQL_RES* result_;
  MYSQL_ROW current_row_;
  unsigned long* lengths_;
  int num_fields_;
  MYSQL_FIELD* fields_;
};
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

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_DATABASE_MYSQL_MYSQL_RESULT_HPP
// -----------------------------------------------------------------------------
