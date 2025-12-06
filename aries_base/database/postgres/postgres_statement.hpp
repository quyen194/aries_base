/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 06:31
  filename:  aries_base/database/postgres/postgres_statement.hpp

  purpose:   PostgreSQL prepared statement implementation
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef aries_base_database_postgres_postgres_statement_hpp
#define aries_base_database_postgres_postgres_statement_hpp
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <cstdint>
#include <string>
#include <vector>

#include <libpq-fe.h>

#include "aries_base/database/interfaces/statement.hpp"
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
/**
 * @brief PostgreSQL prepared statement implementation
 */
class PostgresStatement : public Statement {
 public:
  PostgresStatement(PGconn* conn, const std::string& sql);
  ~PostgresStatement() override;

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
  std::string GetLastError() const override;

 private:
  std::string ConvertPlaceholders(const std::string& sql);

 private:
  PGconn* conn_;
  std::string sql_;
  std::string stmt_name_;
  std::string last_error_;
  std::vector<std::string> param_values_;
  std::vector<int> param_lengths_;
  std::vector<int> param_formats_;
  int param_count_;
  bool prepared_;
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
#endif  // aries_base_database_postgres_postgres_statement_hpp
// -----------------------------------------------------------------------------
