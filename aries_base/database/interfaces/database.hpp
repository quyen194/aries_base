/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/06 21:30
  filename:  aries_base/database/interfaces/database.hpp

  purpose:   Interface for database connections
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_DATABASE_INTERFACES_DATABASE_HPP
#define ARIES_BASE_DATABASE_INTERFACES_DATABASE_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <memory>
#include <string>

#include "aries_base/database/interfaces/result_sets.hpp"
#include "aries_base/database/interfaces/statement.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace database {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
/**
 * @brief Abstract interface for database connections
 *
 * This interface provides a unified API for database operations across
 * different database engines (SQLite, MySQL, PostgreSQL, MSSQL).
 */
class Database {
 public:
  virtual ~Database() = default;

  /**
   * @brief Connect to the database
   * @param connectionString Database-specific connection string
   * @return true if connection successful, false otherwise
   *
   * Connection string formats:
   * - SQLite: "path/to/database.db" or ":memory:"
   * - MySQL: "host=127.0.0.1;port=3306;user=root;password=pass;database=mydb"
   * - PostgreSQL: "host=127.0.0.1;port=5432;user=postgres;password=pass;dbname=mydb"
   * - MSSQL: "Driver={ODBC Driver 17 for SQL Server};Server=127.0.0.1;Database=mydb;UID=sa;PWD=pass"
   */
  virtual bool Connect(const std::string& connectionString) = 0;

  /**
   * @brief Disconnect from the database
   */
  virtual void Disconnect() = 0;

  /**
   * @brief Check if connected to the database
   * @return true if connected, false otherwise
   */
  virtual bool IsConnected() const = 0;

  /**
   * @brief Begin a transaction
   * @return true if transaction started successfully, false otherwise
   */
  virtual bool Begin() = 0;

  /**
   * @brief Commit the current transaction
   * @return true if commit successful, false otherwise
   */
  virtual bool Commit() = 0;

  /**
   * @brief Rollback the current transaction
   * @return true if rollback successful, false otherwise
   */
  virtual bool Rollback() = 0;

  /**
   * @brief Execute a SQL statement and return results
   * @param sql SQL statement to execute
   * @return Result set (may be empty for non-SELECT statements)
   */
  virtual std::unique_ptr<ResultSets> Execute(const std::string& sql) = 0;

  /**
   * @brief Prepare a SQL statement for execution
   * @param sql SQL statement with parameter placeholders (?)
   * @return Prepared statement object
   */
  virtual std::unique_ptr<Statement> Prepare(const std::string& sql) = 0;

  /**
   * @brief Get the last error message
   * @return Error message string
   */
  virtual std::string GetLastError() const = 0;

  /**
   * @brief Get the last inserted row ID (for auto-increment columns)
   * @return Last insert ID, or 0 if not applicable
   */
  virtual int64_t GetLastInsertId() const = 0;

  /**
   * @brief Get the number of rows affected by the last statement
   * @return Number of affected rows
   */
  virtual int64_t GetAffectedRows() const = 0;
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace database
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_DATABASE_INTERFACES_DATABASE_HPP
// -----------------------------------------------------------------------------
