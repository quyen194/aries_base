/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:  quyen19492
  email:   quyen19492@gmail.com

  created:   2025/12/07 07:27
  filename:  aries_base/examples/database/basic.cpp

  purpose:   Basic database functionality example - connect and simple query
*********************************************************************/


// -----------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#endif  // _WIN32

#include <iostream>

#include <aries_base/database/db_factory.hpp>
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::database;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  std::cout << "=== Database Library - Basic Example ===" << std::endl << std::endl;

  // ========================================================================
  // Example 1: SQLite
  // ========================================================================
  std::cout << "--- SQLite Example ---" << std::endl;
  try {
    auto db = DatabaseFactory::Create(DBType::SQLite);

    // Connect to in-memory database
    if (!db->Connect(":memory:")) {
      std::cerr << "Failed to connect: " << db->GetLastError() << std::endl;
      return 1;
    }

    std::cout << "Connected to SQLite database" << std::endl;

    // Create a table
    db->Execute("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT, age INTEGER)");
    std::cout << "Created table 'users'" << std::endl;

    // Insert some data
    db->Execute("INSERT INTO users (name, age) VALUES ('Alice', 30)");
    db->Execute("INSERT INTO users (name, age) VALUES ('Bob', 25)");
    db->Execute("INSERT INTO users (name, age) VALUES ('Charlie', 35)");
    std::cout << "Inserted 3 users" << std::endl;

    // Query data
    auto result = db->Execute("SELECT id, name, age FROM users ORDER BY age");
    if (result) {
      std::cout << "\nQuery Results:" << std::endl;
      std::cout << "ID | Name  | Age" << std::endl;
      std::cout << "---+---------+-----" << std::endl;

      while (result->Next()) {
        int id = result->GetInt(0);
        std::string name = result->GetString(1);
        int age = result->GetInt(2);

        std::cout << id << "  | " << name;
        for (size_t i = name.length(); i < 7; ++i) std::cout << " ";
        std::cout << " | " << age << std::endl;
      }
    }

    db->Disconnect();
    std::cout << "\nDisconnected from SQLite database" << std::endl;

  } catch (const std::exception& e) {
    std::cerr << "SQLite Error: " << e.what() << std::endl;
  }

  std::cout << std::endl;

  // ========================================================================
  // Example 2: PostgreSQL
  // ========================================================================
  std::cout << "--- PostgreSQL Example ---" << std::endl;
  try {
    auto db = DatabaseFactory::Create(DBType::PostgreSQL);

    // Connection string format:
    // "host=127.0.0.1;port=5432;user=postgres;password=mypass;dbname=testdb"
    std::string connStr = "host=127.0.0.1;port=5432;user=postgres;password=postgres;dbname=postgres";

    if (db->Connect(connStr)) {
      std::cout << "Connected to PostgreSQL database" << std::endl;

      // Test query
      auto result = db->Execute("SELECT version()");
      if (result && result->Next()) {
        std::cout << "PostgreSQL version: " << result->GetString(0) << std::endl;
      }

      db->Disconnect();
      std::cout << "Disconnected from PostgreSQL database" << std::endl;
    } else {
      std::cout << "PostgreSQL not available (this is expected if server is not running)" << std::endl;
    }

  } catch (const std::exception& e) {
    std::cout << "PostgreSQL not compiled in or not available: " << e.what() << std::endl;
  }

  std::cout << std::endl;

  // ========================================================================
  // Example 3: MySQL
  // ========================================================================
  std::cout << "--- MySQL Example ---" << std::endl;
  try {
    auto db = DatabaseFactory::Create(DBType::MySQL);

    // Connection string format:
    // "host=127.0.0.1;port=3306;user=root;password=mypass;database=testdb"
    std::string connStr = "host=127.0.0.1;port=3306;user=root;password=root;database=mysql";

    if (db->Connect(connStr)) {
      std::cout << "Connected to MySQL database" << std::endl;

      // Test query
      auto result = db->Execute("SELECT VERSION()");
      if (result && result->Next()) {
        std::cout << "MySQL version: " << result->GetString(0) << std::endl;
      }

      db->Disconnect();
      std::cout << "Disconnected from MySQL database" << std::endl;
    } else {
      std::cout << "MySQL not available (this is expected if server is not running)" << std::endl;
    }

  } catch (const std::exception& e) {
    std::cout << "MySQL not compiled in or not available: " << e.what() << std::endl;
  }

  std::cout << std::endl;

  // ========================================================================
  // Example 4: MSSQL
  // ========================================================================
  std::cout << "--- MSSQL Example ---" << std::endl;
  try {
    auto db = DatabaseFactory::Create(DBType::MSSQL);

    // Connection string format (ODBC):
    // "Driver={ODBC Driver 17 for SQL Server};Server=127.0.0.1;Database=master;UID=sa;PWD=YourPassword"
    std::string connStr = "Driver={ODBC Driver 17 for SQL Server};Server=127.0.0.1;Database=master;UID=sa;PWD=YourPassword";

    if (db->Connect(connStr)) {
      std::cout << "Connected to MSSQL database" << std::endl;

      // Test query
      auto result = db->Execute("SELECT @@VERSION");
      if (result && result->Next()) {
        std::cout << "MSSQL version: " << result->GetString(0) << std::endl;
      }

      db->Disconnect();
      std::cout << "Disconnected from MSSQL database" << std::endl;
    } else {
      std::cout << "MSSQL not available (this is expected if server is not running)" << std::endl;
    }

  } catch (const std::exception& e) {
    std::cout << "MSSQL not compiled in or not available: " << e.what() << std::endl;
  }

  std::cout << "\n=== Example Complete ===" << std::endl;

  return 0;
}
// -----------------------------------------------------------------------------
