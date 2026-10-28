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
   * @brief Transfer ownership of a Database to a MSSQLDatabase pointer
   * @param db Database instance; emptied on success, untouched on failure
   * @return MSSQLDatabase pointer, or nullptr if db is not a MSSQLDatabase
   */
  static std::unique_ptr<MSSQLDatabase> ToMSSQL(std::unique_ptr<Database>& db);
#endif  // ARIES_DB_MSSQL

#ifdef ARIES_DB_MYSQL
  /**
   * @brief Transfer ownership of a Database to a MySQLDatabase pointer
   * @param db Database instance; emptied on success, untouched on failure
   * @return MySQLDatabase pointer, or nullptr if db is not a MySQLDatabase
   */
  static std::unique_ptr<MySQLDatabase> ToMySQL(std::unique_ptr<Database>& db);
#endif  // ARIES_DB_MYSQL

#ifdef ARIES_DB_POSTGRES
  /**
   * @brief Transfer ownership of a Database to a PostgresDatabase pointer
   * @param db Database instance; emptied on success, untouched on failure
   * @return PostgresDatabase pointer, or nullptr if db is not a PostgresDatabase
   */
  static std::unique_ptr<PostgresDatabase> ToPostgres(std::unique_ptr<Database>& db);
#endif  // ARIES_DB_POSTGRES

#ifdef ARIES_DB_SQLITE
  /**
   * @brief Transfer ownership of a Database to a SQLiteDatabase pointer
   * @param db Database instance; emptied on success, untouched on failure
   * @return SQLiteDatabase pointer, or nullptr if db is not a SQLiteDatabase
   */
  static std::unique_ptr<SQLiteDatabase> ToSQLite(std::unique_ptr<Database>& db);
#endif  // ARIES_DB_SQLITE

#ifdef ARIES_DB_MSSQL
  /**
   * @brief Transfer ownership of a Statement to a MSSQLStatement pointer
   * @param stmt Statement instance; emptied on success, untouched on failure
   * @return MSSQLStatement pointer, or nullptr if stmt is not a MSSQLStatement
   */
  static std::unique_ptr<MSSQLStatement> ToMSSQL(std::unique_ptr<Statement>& stmt);
#endif  // ARIES_DB_MSSQL

#ifdef ARIES_DB_MYSQL
  /**
   * @brief Transfer ownership of a Statement to a MySQLStatement pointer
   * @param stmt Statement instance; emptied on success, untouched on failure
   * @return MySQLStatement pointer, or nullptr if stmt is not a MySQLStatement
   */
  static std::unique_ptr<MySQLStatement> ToMySQL(std::unique_ptr<Statement>& stmt);
#endif  // ARIES_DB_MYSQL

#ifdef ARIES_DB_POSTGRES
  /**
   * @brief Transfer ownership of a Statement to a PostgresStatement pointer
   * @param stmt Statement instance; emptied on success, untouched on failure
   * @return PostgresStatement pointer, or nullptr if stmt is not a PostgresStatement
   */
  static std::unique_ptr<PostgresStatement> ToPostgres(std::unique_ptr<Statement>& stmt);
#endif  // ARIES_DB_POSTGRES

#ifdef ARIES_DB_SQLITE
  /**
   * @brief Transfer ownership of a Statement to a SQLiteStatement pointer
   * @param stmt Statement instance; emptied on success, untouched on failure
   * @return SQLiteStatement pointer, or nullptr if stmt is not a SQLiteStatement
   */
  static std::unique_ptr<SQLiteStatement> ToSQLite(std::unique_ptr<Statement>& stmt);
#endif  // ARIES_DB_SQLITE

#ifdef ARIES_DB_MSSQL
  /**
   * @brief Transfer ownership of a ResultSets to a MSSQLResult pointer
   * @param result ResultSets instance; emptied on success, untouched on failure
   * @return MSSQLResult pointer, or nullptr if result is not a MSSQLResult
   */
  static std::unique_ptr<MSSQLResult> ToMSSQL(std::unique_ptr<ResultSets>& result);
#endif  // ARIES_DB_MSSQL

#ifdef ARIES_DB_MYSQL
  /**
   * @brief Transfer ownership of a ResultSets to a MySQLResult pointer
   * @param result ResultSets instance; emptied on success, untouched on failure
   * @return MySQLResult pointer, or nullptr if result is not a MySQLResult
   */
  static std::unique_ptr<MySQLResult> ToMySQL(std::unique_ptr<ResultSets>& result);
#endif  // ARIES_DB_MYSQL

#ifdef ARIES_DB_POSTGRES
  /**
   * @brief Transfer ownership of a ResultSets to a PostgresResult pointer
   * @param result ResultSets instance; emptied on success, untouched on failure
   * @return PostgresResult pointer, or nullptr if result is not a PostgresResult
   */
  static std::unique_ptr<PostgresResult> ToPostgres(std::unique_ptr<ResultSets>& result);
#endif  // ARIES_DB_POSTGRES

#ifdef ARIES_DB_SQLITE
  /**
   * @brief Transfer ownership of a ResultSets to a SQLiteResult pointer
   * @param result ResultSets instance; emptied on success, untouched on failure
   * @return SQLiteResult pointer, or nullptr if result is not a SQLiteResult
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
