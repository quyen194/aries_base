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
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace database {
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
