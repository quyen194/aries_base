/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/06 22:55
  filename:  aries_base/database/mssql/mssql_result.hpp

  purpose:   MSSQL result set implementation
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_DATABASE_MSSQL_MSSQL_RESULT_HPP
#define ARIES_BASE_DATABASE_MSSQL_MSSQL_RESULT_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <cstdint>
#include <string>
#include <vector>

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#include "aries_base/database/interfaces/result_sets.hpp"
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace database {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace mssql {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
/**
 * @brief MSSQL result set implementation
 */
class MSSQLResult : public ResultSets {
 public:
  MSSQLResult(SQLHSTMT hstmt, bool ownsStatement = true);
  ~MSSQLResult() override;

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
  std::string GetODBCError();

 private:
  SQLHSTMT hstmt_;
  bool owns_statement_;
  bool has_row_;
  SQLSMALLINT num_cols_;
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace mssql
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace database
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_DATABASE_MSSQL_MSSQL_RESULT_HPP
// -----------------------------------------------------------------------------
