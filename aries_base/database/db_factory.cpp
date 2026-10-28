/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Games: Elefant Blaster
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/06 21:39
  filename:  aries_base/database/db_factory.cpp

  purpose:   Factory for creating database instances
*********************************************************************/


// -----------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#endif  // _WIN32

#include <stdexcept>

#include "aries_base/database/db_factory.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace {
template <typename To, typename From>
std::unique_ptr<To> TransferAs(std::unique_ptr<From>& from) {
  auto* raw = dynamic_cast<To*>(from.get());
  if (!raw) {
    // wrong type: db keeps ownership
    return nullptr;
  }
  // ownership moves to the returned pointer
  from.release();
  return std::unique_ptr<To>(raw);
}
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace database {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

std::unique_ptr<Database> DatabaseFactory::Create(DBType type) {
  switch (type) {
    case DBType::SQLite:
#ifdef ARIES_DB_SQLITE
      return std::make_unique<sqlite::SQLiteDatabase>();
#else
      throw std::runtime_error("SQLite support not compiled in. Define ARIES_DB_SQLITE and link sqlite3.");
#endif

    case DBType::MySQL:
#ifdef ARIES_DB_MYSQL
      return std::make_unique<mysql::MySQLDatabase>();
#else
      throw std::runtime_error("MySQL support not compiled in. Define ARIES_DB_MYSQL and link libmysqlclient.");
#endif

    case DBType::PostgreSQL:
#ifdef ARIES_DB_POSTGRES
      return std::make_unique<postgres::PostgresDatabase>();
#else
      throw std::runtime_error("PostgreSQL support not compiled in. Define ARIES_DB_POSTGRES and link libpq.");
#endif

    case DBType::MSSQL:
#ifdef ARIES_DB_MSSQL
      return std::make_unique<mssql::MSSQLDatabase>();
#else
      throw std::runtime_error("MSSQL support not compiled in. Define ARIES_DB_MSSQL and link ODBC.");
#endif

    default:
      throw std::runtime_error("Unknown database type");
  }
}
// -----------------------------------------------------------------------------

#ifdef ARIES_DB_MSSQL
std::unique_ptr<MSSQLDatabase> DatabaseFactory::ToMSSQL(
    std::unique_ptr<Database>& db) {
  return TransferAs<MSSQLDatabase>(db);
}
#endif  // ARIES_DB_MSSQL
// -----------------------------------------------------------------------------

#ifdef ARIES_DB_MYSQL
std::unique_ptr<MySQLDatabase> DatabaseFactory::ToMySQL(
    std::unique_ptr<Database>& db) {
  return TransferAs<MySQLDatabase>(db);
}
#endif  // ARIES_DB_MYSQL
// -----------------------------------------------------------------------------

#ifdef ARIES_DB_POSTGRES
std::unique_ptr<PostgresDatabase> DatabaseFactory::ToPostgres(
    std::unique_ptr<Database>& db) {
  return TransferAs<PostgresDatabase>(db);
}
#endif  // ARIES_DB_POSTGRES
// -----------------------------------------------------------------------------

#ifdef ARIES_DB_SQLITE
std::unique_ptr<SQLiteDatabase> DatabaseFactory::ToSQLite(
    std::unique_ptr<Database>& db) {
  return TransferAs<SQLiteDatabase>(db);
}
#endif  // ARIES_DB_SQLITE
// -----------------------------------------------------------------------------

#ifdef ARIES_DB_MSSQL
std::unique_ptr<MSSQLStatement> DatabaseFactory::ToMSSQL(
    std::unique_ptr<Statement>& stmt) {
  return TransferAs<MSSQLStatement>(stmt);
}
#endif  // ARIES_DB_MSSQL
// -----------------------------------------------------------------------------

#ifdef ARIES_DB_MYSQL
std::unique_ptr<MySQLStatement> DatabaseFactory::ToMySQL(
    std::unique_ptr<Statement>& stmt) {
  return TransferAs<MySQLStatement>(stmt);
}
#endif  // ARIES_DB_MYSQL
// -----------------------------------------------------------------------------

#ifdef ARIES_DB_POSTGRES
std::unique_ptr<PostgresStatement> DatabaseFactory::ToPostgres(
    std::unique_ptr<Statement>& stmt) {
  return TransferAs<PostgresStatement>(stmt);
}
#endif  // ARIES_DB_POSTGRES
// -----------------------------------------------------------------------------

#ifdef ARIES_DB_SQLITE
std::unique_ptr<SQLiteStatement> DatabaseFactory::ToSQLite(
    std::unique_ptr<Statement>& stmt) {
  return TransferAs<SQLiteStatement>(stmt);
}
#endif  // ARIES_DB_SQLITE
// -----------------------------------------------------------------------------

#ifdef ARIES_DB_MSSQL
std::unique_ptr<MSSQLResult> DatabaseFactory::ToMSSQL(
    std::unique_ptr<ResultSets>& result) {
  return TransferAs<MSSQLResult>(result);
}
#endif  // ARIES_DB_MSSQL
// -----------------------------------------------------------------------------

#ifdef ARIES_DB_MYSQL
std::unique_ptr<MySQLResult> DatabaseFactory::ToMySQL(
    std::unique_ptr<ResultSets>& result) {
  return TransferAs<MySQLResult>(result);
}
#endif  // ARIES_DB_MYSQL
// -----------------------------------------------------------------------------

#ifdef ARIES_DB_POSTGRES
std::unique_ptr<PostgresResult> DatabaseFactory::ToPostgres(
    std::unique_ptr<ResultSets>& result) {
  return TransferAs<PostgresResult>(result);
}
#endif  // ARIES_DB_POSTGRES
// -----------------------------------------------------------------------------

#ifdef ARIES_DB_SQLITE
std::unique_ptr<SQLiteResult> DatabaseFactory::ToSQLite(
    std::unique_ptr<ResultSets>& result) {
  return TransferAs<SQLiteResult>(result);
}
#endif  // ARIES_DB_SQLITE
// -----------------------------------------------------------------------------

const char* DatabaseFactory::GetDatabaseTypeName(DBType type) {
  switch (type) {
    case DBType::SQLite:     return "SQLite";
    case DBType::MySQL:      return "MySQL";
    case DBType::PostgreSQL: return "PostgreSQL";
    case DBType::MSSQL:      return "MSSQL";
    default:                 return "Unknown";
  }
}
// -----------------------------------------------------------------------------

DBType DatabaseFactory::GetDatabaseTypeFromName(const std::string& name) {
  if (name == "SQLite") return DBType::SQLite;
  if (name == "MySQL") return DBType::MySQL;
  if (name == "PostgreSQL") return DBType::PostgreSQL;
  if (name == "MSSQL") return DBType::MSSQL;
  return DBType::Unknown;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace database
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------
