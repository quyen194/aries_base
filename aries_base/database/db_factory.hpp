/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/06 21:16
  filename:  aries_base/database/db_factory.hpp

  purpose:   Factory for creating database instances
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_DATABASE_DB_FACTORY_HPP
#define ARIES_BASE_DATABASE_DB_FACTORY_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <memory>
#include <string>

#include "aries_base/database/interfaces/database.hpp"

#ifdef ARIES_DB_MSSQL
#include "aries_base/database/mssql/mssql_database.hpp"
#endif  // ARIES_DB_MSSQL

#ifdef ARIES_DB_MYSQL
#include "aries_base/database/mysql/mysql_database.hpp"
#endif  // ARIES_DB_MYSQL

#ifdef ARIES_DB_POSTGRES
#include "aries_base/database/postgres/postgres_database.hpp"
#endif  // ARIES_DB_POSTGRES

#ifdef ARIES_DB_SQLITE
#include "aries_base/database/sqlite/sqlite_database.hpp"
#endif  // ARIES_DB_SQLITE
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace database {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#ifdef ARIES_DB_MSSQL
using namespace mssql;
#endif  // ARIES_DB_MSSQL
#ifdef ARIES_DB_MYSQL
using namespace mysql;
#endif  // ARIES_DB_MYSQL
#ifdef ARIES_DB_POSTGRES
using namespace postgres;
#endif  // ARIES_DB_POSTGRES
#ifdef ARIES_DB_SQLITE
using namespace sqlite;
#endif  // ARIES_DB_SQLITE
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
/**
 * @brief Enumeration of supported database types
 */
enum class DBType {
  Unknown,      ///< Unknown database type
  SQLite,       ///< SQLite database
  MySQL,        ///< MySQL/MariaDB database
  PostgreSQL,   ///< PostgreSQL database
  MSSQL         ///< Microsoft SQL Server (via ODBC)
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
class DatabaseFactory {
 public:
  /**
   * @brief Factory function to create database instances
   *
   * @param type The type of database to create
   * @return Unique pointer to the created database instance
   *
   * @throws std::runtime_error if the database type is not supported or not compiled in
   *
   * @example
   * @code
   * auto db = CreateDatabase(DBType::PostgreSQL);
   * db->Connect("host=127.0.0.1;port=5432;user=test;password=123;dbname=mydb");
   * @endcode
   */
  static std::unique_ptr<Database> Create(DBType type);

#ifdef ARIES_DB_MSSQL
  /**
   * @brief Convert a generic Database pointer to a MySQLDatabase pointer
   * @param db Unique pointer to a Database instance
   * @return Unique pointer to a MySQLDatabase instance
   * @throws std::runtime_error if the conversion fails
   */
  static std::unique_ptr<MSSQLDatabase> ToMSSQL(std::unique_ptr<Database>& db);
#endif  // ARIES_DB_MSSQL

#ifdef ARIES_DB_MYSQL
  /**
   * @brief Convert a generic Database pointer to a MySQLDatabase pointer
   * @param db Unique pointer to a Database instance
   * @return Unique pointer to a MySQLDatabase instance
   * @throws std::runtime_error if the conversion fails
   */
  static std::unique_ptr<MySQLDatabase> ToMySQL(std::unique_ptr<Database>& db);
#endif  // ARIES_DB_MYSQL

#ifdef ARIES_DB_POSTGRES
  /**
   * @brief Convert a generic Database pointer to a PostgresDatabase pointer
   * @param db Unique pointer to a Database instance
   * @return Unique pointer to a PostgresDatabase instance
   * @throws std::runtime_error if the conversion fails
   */
  static std::unique_ptr<PostgresDatabase> ToPostgres(std::unique_ptr<Database>& db);
#endif  // ARIES_DB_POSTGRES

#ifdef ARIES_DB_SQLITE
  /**
   * @brief Convert a generic Database pointer to a SQLiteDatabase pointer
   * @param db Unique pointer to a Database instance
   * @return Unique pointer to a SQLiteDatabase instance
   * @throws std::runtime_error if the conversion fails
   */
  static std::unique_ptr<SQLiteDatabase> ToSQLite(std::unique_ptr<Database>& db);
#endif  // ARIES_DB_SQLITE

#ifdef ARIES_DB_MSSQL
  /**
   * @brief Convert a generic Statement pointer to a MSSQLStatement pointer
   * @param stmt Unique pointer to a Statement instance
   * @return Unique pointer to a MSSQLStatement instance
   * @throws std::runtime_error if the conversion fails
   */
  static std::unique_ptr<MSSQLStatement> ToMSSQL(std::unique_ptr<Statement>& stmt);
#endif  // ARIES_DB_MSSQL

#ifdef ARIES_DB_MYSQL
  /**
   * @brief Convert a generic Statement pointer to a MySQLStatement pointer
   * @param stmt Unique pointer to a Statement instance
   * @return Unique pointer to a MySQLStatement instance
   * @throws std::runtime_error if the conversion fails
   */
  static std::unique_ptr<MySQLStatement> ToMySQL(std::unique_ptr<Statement>& stmt);
#endif  // ARIES_DB_MYSQL

#ifdef ARIES_DB_POSTGRES
  /**
   * @brief Convert a generic Statement pointer to a PostgresStatement pointer
   * @param stmt Unique pointer to a Statement instance
   * @return Unique pointer to a PostgresStatement instance
   * @throws std::runtime_error if the conversion fails
   */
  static std::unique_ptr<PostgresStatement> ToPostgres(std::unique_ptr<Statement>& stmt);
#endif  // ARIES_DB_POSTGRES

#ifdef ARIES_DB_SQLITE
  /**
   * @brief Convert a generic Statement pointer to a SQLiteStatement pointer
   * @param stmt Unique pointer to a Statement instance
   * @return Unique pointer to a SQLiteStatement instance
   * @throws std::runtime_error if the conversion fails
   */
  static std::unique_ptr<SQLiteStatement> ToSQLite(std::unique_ptr<Statement>& stmt);
#endif  // ARIES_DB_SQLITE

#ifdef ARIES_DB_MSSQL
  /**
   * @brief Convert a generic ResultSets pointer to a MSSQLResult pointer
   * @param result Unique pointer to a ResultSets instance
   * @return Unique pointer to a MSSQLResult instance
   * @throws std::runtime_error if the conversion fails
   */
  static std::unique_ptr<MSSQLResult> ToMSSQL(std::unique_ptr<ResultSets>& result);
#endif  // ARIES_DB_MSSQL

#ifdef ARIES_DB_MYSQL
  /**
   * @brief Convert a generic ResultSets pointer to a MySQLResult pointer
   * @param result Unique pointer to a ResultSets instance
   * @return Unique pointer to a MySQLResult instance
   * @throws std::runtime_error if the conversion fails
   */
  static std::unique_ptr<MySQLResult> ToMySQL(std::unique_ptr<ResultSets>& result);
#endif  // ARIES_DB_MYSQL

#ifdef ARIES_DB_POSTGRES
  /**
   * @brief Convert a generic ResultSets pointer to a PostgresResult pointer
   * @param result Unique pointer to a ResultSets instance
   * @return Unique pointer to a PostgresResult instance
   * @throws std::runtime_error if the conversion fails
   */
  static std::unique_ptr<PostgresResult> ToPostgres(std::unique_ptr<ResultSets>& result);
#endif  // ARIES_DB_POSTGRES

#ifdef ARIES_DB_SQLITE
  /**
   * @brief Convert a generic ResultSets pointer to a SQLiteResult pointer
   * @param result Unique pointer to a ResultSets instance
   * @return Unique pointer to a SQLiteResult instance
   * @throws std::runtime_error if the conversion fails
   */
  static std::unique_ptr<SQLiteResult> ToSQLite(std::unique_ptr<ResultSets>& result);
#endif  // ARIES_DB_SQLITE

  /**
   * @brief Get the name of a database type
   * @param type Database type
   * @return String representation of the database type
   */
  static const char* GetDatabaseTypeName(DBType type);

  /**
   * @brief Get the database type from its name
   * @param name String representation of the database type
   * @return Corresponding DBType enum value
   * @throws std::runtime_error if the name does not match any known database type
   */
  static DBType GetDatabaseTypeFromName(const std::string& name);
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace database
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_DATABASE_DB_FACTORY_HPP
// -----------------------------------------------------------------------------
