/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 06:34
  filename:  aries_base/database/postgres/postgres_database.hpp

  purpose:   PostgreSQL database implementation
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_DATABASE_POSTGRES_POSTGRES_DATABASE_HPP
#define ARIES_BASE_DATABASE_POSTGRES_POSTGRES_DATABASE_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <string>
#include <memory>

#include <libpq-fe.h>

#include "aries_base/database/interfaces/database.hpp"
#include "aries_base/database/postgres/postgres_statement.hpp"
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
 * @brief PostgreSQL database implementation
 */
class PostgresDatabase : public Database {
 public:
  PostgresDatabase();
  ~PostgresDatabase() override;

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

  // PostgreSQL-specific extensions

  /**
   * @brief Execute COPY command for bulk data operations
   * @param sql COPY command
   * @return true if successful
   */
  bool ExecuteCopy(const std::string& sql);

  /**
   * @brief Get PostgreSQL server version
   * @return Version string
   */
  std::string GetServerVersion();

  /**
   * @brief Listen for notifications on a channel
   * @param channel Channel name
   * @return true if successful
   */
  bool Listen(const std::string& channel);

  /**
   * @brief Unlisten from a channel
   * @param channel Channel name
   * @return true if successful
   */
  bool Unlisten(const std::string& channel);

  /**
   * @brief Send a notification to a channel
   * @param channel Channel name
   * @param payload Notification payload
   * @return true if successful
   */
  bool Notify(const std::string& channel, const std::string& payload = "");

  // Internal access
  PGconn* GetHandle() { return conn_; }

 private:
  std::string ConvertToPostgresConnString(const std::string& connStr);

 private:
  PGconn* conn_;
  bool connected_;
  std::string last_error_;
  int64_t last_affected_rows_;
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
#endif  // ARIES_BASE_DATABASE_POSTGRES_POSTGRES_DATABASE_HPP
// -----------------------------------------------------------------------------
