/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 06:11
  filename:  aries_base/database/mssql/mssql_database.hpp

  purpose:   Microsoft SQL Server database implementation (via ODBC)
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_DATABASE_MSSQL_MSSQL_DATABASE_HPP
#define ARIES_BASE_DATABASE_MSSQL_MSSQL_DATABASE_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <cstdint>
#include <string>
#include <memory>
#include <vector>

#include <sql.h>
#include <sqlext.h>

#include "aries_base/database/interfaces/database.hpp"
#include "aries_base/database/mssql/mssql_statement.hpp"
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
 * @brief Microsoft SQL Server database implementation (via ODBC)
 */
class MSSQLDatabase : public Database {
 public:
  MSSQLDatabase();
  ~MSSQLDatabase() override;

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

  // MSSQL-specific extensions

  /**
   * @brief Execute a stored procedure
   * @param procName Procedure name
   * @param params Parameters for the procedure
   * @return Result set
   */
  std::unique_ptr<ResultSets> ExecuteStoredProcedure(
      const std::string& procName,
      const std::vector<std::string>& params = {});

  /**
   * @brief Get MSSQL server version
   * @return Version string
   */
  std::string GetServerVersion();

  // Internal access
  SQLHDBC GetHandle() { return hdbc_; }
  SQLHENV GetEnv() { return henv_; }

 private:
  std::string GetODBCError(SQLSMALLINT handleType, SQLHANDLE handle);

 private:
  SQLHENV henv_;    // Environment handle
  SQLHDBC hdbc_;    // Connection handle
  bool connected_;
  std::string last_error_;
  int64_t last_affected_rows_;
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
#endif  // ARIES_BASE_DATABASE_MSSQL_MSSQL_DATABASE_HPP
// -----------------------------------------------------------------------------
