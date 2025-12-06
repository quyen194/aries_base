/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 06:39
  filename:  aries_base/database/postgres/postgres_database.cpp

  purpose:   PostgreSQL database implementation
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cstring>
#include <random>
#include <stdexcept>
#include <sstream>

#include <libpq-fe.h>

#include "aries_base/database/postgres/postgres_database.hpp"
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

PostgresDatabase::PostgresDatabase()
    : conn_(nullptr), connected_(false), last_affected_rows_(0) {}
// -----------------------------------------------------------------------------

PostgresDatabase::~PostgresDatabase() {
  Disconnect();
}
// -----------------------------------------------------------------------------

std::string PostgresDatabase::ConvertToPostgresConnString(
    const std::string& connStr) {
  // Input format: "host=127.0.0.1;port=5432;user=postgres;password=pass;dbname=mydb"
  // Output format: "host=127.0.0.1 port=5432 user=postgres password=pass dbname=mydb"

  std::string result = connStr;
  size_t pos = 0;
  while ((pos = result.find(';', pos)) != std::string::npos) {
    result[pos] = ' ';
  }

  return result;
}
// -----------------------------------------------------------------------------

bool PostgresDatabase::Connect(const std::string& connectionString) {
  if (connected_) {
    Disconnect();
  }

  std::string pgConnStr = ConvertToPostgresConnString(connectionString);
  conn_ = PQconnectdb(pgConnStr.c_str());

  if (PQstatus(conn_) != CONNECTION_OK) {
    last_error_ = PQerrorMessage(conn_);
    PQfinish(conn_);
    conn_ = nullptr;
    connected_ = false;
    return false;
  }

  // Set client encoding to UTF8
  PQexec(conn_, "SET CLIENT_ENCODING TO 'UTF8'");

  connected_ = true;
  last_error_.clear();
  return true;
}
// -----------------------------------------------------------------------------

void PostgresDatabase::Disconnect() {
  if (conn_) {
    PQfinish(conn_);
    conn_ = nullptr;
  }
  connected_ = false;
}
// -----------------------------------------------------------------------------

bool PostgresDatabase::IsConnected() const {
  return connected_ && conn_ && PQstatus(conn_) == CONNECTION_OK;
}
// -----------------------------------------------------------------------------

bool PostgresDatabase::Begin() {
  auto result = Execute("BEGIN");
  return result != nullptr;
}
// -----------------------------------------------------------------------------

bool PostgresDatabase::Commit() {
  auto result = Execute("COMMIT");
  return result != nullptr;
}
// -----------------------------------------------------------------------------

bool PostgresDatabase::Rollback() {
  auto result = Execute("ROLLBACK");
  return result != nullptr;
}
// -----------------------------------------------------------------------------

std::unique_ptr<ResultSets> PostgresDatabase::Execute(const std::string& sql) {
  if (!connected_) {
    last_error_ = "Not connected to database";
    return nullptr;
  }

  PGresult* result = PQexec(conn_, sql.c_str());

  if (!result) {
    last_error_ = PQerrorMessage(conn_);
    return nullptr;
  }

  ExecStatusType status = PQresultStatus(result);

  if (status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK) {
    // Get affected rows
    const char* affectedRows = PQcmdTuples(result);
    if (affectedRows && affectedRows[0]) {
      last_affected_rows_ = std::stoll(affectedRows);
    } else {
      last_affected_rows_ = 0;
    }

    last_error_.clear();
    return std::make_unique<PostgresResult>(result);
  } else {
    last_error_ = PQerrorMessage(conn_);
    PQclear(result);
    return nullptr;
  }
}
// -----------------------------------------------------------------------------

std::unique_ptr<Statement> PostgresDatabase::Prepare(const std::string& sql) {
  if (!connected_) {
    last_error_ = "Not connected to database";
    return nullptr;
  }

  try {
    return std::make_unique<PostgresStatement>(conn_, sql);
  } catch (const std::exception& e) {
    last_error_ = e.what();
    return nullptr;
  }
}
// -----------------------------------------------------------------------------

std::string PostgresDatabase::GetLastError() const {
  return last_error_;
}
// -----------------------------------------------------------------------------

int64_t PostgresDatabase::GetLastInsertId() const {
  if (!connected_) {
    return 0;
  }

  // PostgreSQL doesn't have a direct last_insert_id() like MySQL
  // Applications should use RETURNING clause or currval()
  PGresult* result = PQexec(conn_, "SELECT lastval()");

  if (result && PQresultStatus(result) == PGRES_TUPLES_OK &&
      PQntuples(result) > 0) {
    int64_t id = std::stoll(PQgetvalue(result, 0, 0));
    PQclear(result);
    return id;
  }

  if (result) {
    PQclear(result);
  }

  return 0;
}
// -----------------------------------------------------------------------------

int64_t PostgresDatabase::GetAffectedRows() const {
  return last_affected_rows_;
}
// -----------------------------------------------------------------------------

bool PostgresDatabase::ExecuteCopy(const std::string& sql) {
  auto result = Execute(sql);
  return result != nullptr;
}
// -----------------------------------------------------------------------------

std::string PostgresDatabase::GetServerVersion() {
  if (!connected_) {
    return "";
  }

  int version = PQserverVersion(conn_);
  return std::to_string(version);
}
// -----------------------------------------------------------------------------

bool PostgresDatabase::Listen(const std::string& channel) {
  std::string sql = "LISTEN " + channel;
  auto result = Execute(sql);
  return result != nullptr;
}
// -----------------------------------------------------------------------------

bool PostgresDatabase::Unlisten(const std::string& channel) {
  std::string sql = "UNLISTEN " + channel;
  auto result = Execute(sql);
  return result != nullptr;
}
// -----------------------------------------------------------------------------

bool PostgresDatabase::Notify(const std::string& channel,
                              const std::string& payload) {
  std::string sql = "NOTIFY " + channel;
  if (!payload.empty()) {
    sql += ", '" + payload + "'";
  }
  auto result = Execute(sql);
  return result != nullptr;
}
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
