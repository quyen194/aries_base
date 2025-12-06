/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 06:20
  filename:  aries_base/database/mysql/mysql_database.hpp

  purpose:   MySQL database implementation
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_DATABASE_MYSQL_MYSQL_DATABASE_HPP
#define ARIES_BASE_DATABASE_MYSQL_MYSQL_DATABASE_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <cstdint>
#include <string>

#include <mysql/mysql.h>

#include "aries_base/database/interfaces/database.hpp"
#include "aries_base/database/mysql/mysql_statement.hpp"
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
 * @brief MySQL/MariaDB database implementation
 */
class MySQLDatabase : public Database {
 public:
  MySQLDatabase();
  ~MySQLDatabase() override;

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

  // MySQL-specific extensions

  /**
   * @brief Execute SHOW TABLES command
   * @return Result set with table names
   */
  std::unique_ptr<ResultSets> ShowTables();

  /**
   * @brief Execute SHOW DATABASES command
   * @return Result set with database names
   */
  std::unique_ptr<ResultSets> ShowDatabases();

  /**
   * @brief Get MySQL server version
   * @return Version string
   */
  std::string GetServerVersion();

  /**
   * @brief Set character set
   * @param charset Character set name (e.g., "utf8mb4")
   * @return true if successful
   */
  bool SetCharset(const std::string& charset);

  // Internal access
  MYSQL* GetHandle() { return mysql_; }

 private:
  bool ParseConnectionString(const std::string& connStr,
                             std::string& host,
                             int& port,
                             std::string& user,
                             std::string& password,
                             std::string& database);

 private:
  MYSQL* mysql_;
  bool connected_;
  std::string last_error_;
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
#endif  // ARIES_BASE_DATABASE_MYSQL_MYSQL_DATABASE_HPP
// -----------------------------------------------------------------------------
