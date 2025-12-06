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

#ifdef ARIES_DB_SQLITE
#include "aries_base/database/sqlite/sqlite_database.hpp"
#endif

#ifdef ARIES_DB_MYSQL
#include "aries_base/database/mysql/mysql_database.hpp"
#endif

#ifdef ARIES_DB_POSTGRES
#include "aries_base/database/postgres/postgres_database.hpp"
#endif

#ifdef ARIES_DB_MSSQL
#include "aries_base/database/mssql/mssql_database.hpp"
#endif

#include "aries_base/database/db_factory.hpp"
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
