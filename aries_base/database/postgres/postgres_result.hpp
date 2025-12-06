/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 06:30
  filename:  aries_base/database/postgres/postgres_result.hpp

  purpose:   PostgreSQL result set implementation
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_DATABASE_POSTGRES_POSTGRES_RESULT_HPP
#define ARIES_BASE_DATABASE_POSTGRES_POSTGRES_RESULT_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <cstdint>
#include <string>
#include <vector>

#include <libpq-fe.h>

#include "aries_base/database/interfaces/result_sets.hpp"
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
/**
 * @brief PostgreSQL result set implementation
 */
class PostgresResult : public ResultSets {
 public:
  PostgresResult(PGresult* result);
  ~PostgresResult() override;

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
  PGresult* result_;
  int current_row_;
  int num_rows_;
  int num_fields_;
};
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

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_DATABASE_POSTGRES_POSTGRES_RESULT_HPP
// -----------------------------------------------------------------------------
