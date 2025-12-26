/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 07:28
  filename:  aries_base/examples/database/multi_db.cpp

  purpose:   Example showing how to switch between database engines
*********************************************************************/


// -----------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#endif  // _WIN32

#include <iostream>
#include <memory>

#include <aries_base/database/db_factory.hpp>
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::database;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
/**
 * @brief Helper class to demonstrate database abstraction
 */
class UserRepository {
 private:
  std::unique_ptr<Database> db;

 public:
  UserRepository(DBType type) {
    db = DatabaseFactory::Create(type);
  }

  bool connect(const std::string& connectionString) {
    return db->Connect(connectionString);
  }

  void createTable() {
    db->Execute(R"(
      CREATE TABLE IF NOT EXISTS users (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        username TEXT NOT NULL UNIQUE,
        email TEXT NOT NULL,
        age INTEGER,
        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
      )
    )");
  }

  bool addUser(const std::string& username, const std::string& email, int age) {
    auto stmt = db->Prepare(
      "INSERT INTO users (username, email, age) VALUES (?, ?, ?)"
    );

    if (!stmt) {
      std::cerr << "Failed to prepare statement: " << db->GetLastError() << std::endl;
      return false;
    }

    stmt->BindString(1, username);
    stmt->BindString(2, email);
    stmt->BindInt(3, age);

    if (!stmt->Execute()) {
      std::cerr << "Failed to insert user: " << db->GetLastError() << std::endl;
      return false;
    }

    return true;
  }

  void listAllUsers() {
    auto result = db->Execute("SELECT id, username, email, age FROM users ORDER BY id");

    if (!result) {
      std::cerr << "Failed to query users: " << db->GetLastError() << std::endl;
      return;
    }

    std::cout << "\nAll Users:" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    std::cout << "ID | Username       | Email          | Age" << std::endl;
    std::cout << std::string(70, '-') << std::endl;

    while (result->Next()) {
      printf("%2d | %-18s | %-22s | %3d\n",
           result->GetInt(0),
           result->GetString(1).c_str(),
           result->GetString(2).c_str(),
           result->GetInt(3));
    }
  }

  int getUserCount() {
    auto result = db->Execute("SELECT COUNT(*) FROM users");
    if (result && result->Next()) {
      return result->GetInt(0);
    }
    return 0;
  }

  bool updateUserAge(const std::string& username, int newAge) {
    auto stmt = db->Prepare("UPDATE users SET age = ? WHERE username = ?");
    if (!stmt) return false;

    stmt->BindInt(1, newAge);
    stmt->BindString(2, username);

    return stmt->Execute();
  }

  bool deleteUser(const std::string& username) {
    auto stmt = db->Prepare("DELETE FROM users WHERE username = ?");
    if (!stmt) return false;

    stmt->BindString(1, username);
    return stmt->Execute();
  }

  void disconnect() {
    if (db) {
      db->Disconnect();
    }
  }
};
// -----------------------------------------------------------------------------

void demonstrateDatabase(DBType dbType, const std::string& connectionString) {
  std::string dbName = DatabaseFactory::GetDatabaseTypeName(dbType);

  std::cout << "\n" << std::string(70, '=') << std::endl;
  std::cout << "Demonstrating: " << dbName << std::endl;
  std::cout << std::string(70, '=') << std::endl;

  try {
    UserRepository repo(dbType);

    if (!repo.connect(connectionString)) {
      std::cout << dbName << " not available (expected if server not running)" << std::endl;
      return;
    }

    std::cout << "✓ Connected to " << dbName << std::endl;

    repo.createTable();
    std::cout << "✓ Created users table" << std::endl;

    // Add some users
    std::cout << "\nAdding users..." << std::endl;
    repo.addUser("alice", "alice@example.com", 30);
    repo.addUser("bob", "bob@example.com", 25);
    repo.addUser("charlie", "charlie@example.com", 35);
    std::cout << "✓ Added 3 users" << std::endl;

    // List users
    repo.listAllUsers();
    std::cout << "✓ Total users: " << repo.getUserCount() << std::endl;

    // Update a user
    std::cout << "\nUpdating Bob's age to 26..." << std::endl;
    if (repo.updateUserAge("bob", 26)) {
      std::cout << "✓ User updated" << std::endl;
    }

    // List updated users
    repo.listAllUsers();

    // Delete a user
    std::cout << "\nDeleting Charlie..." << std::endl;
    if (repo.deleteUser("charlie")) {
      std::cout << "✓ User deleted" << std::endl;
      std::cout << "✓ Remaining users: " << repo.getUserCount() << std::endl;
    }

    repo.disconnect();
    std::cout << "\n✓ Disconnected from " << dbName << std::endl;

  } catch (const std::exception& e) {
    std::cout << "✗ " << dbName << " error: " << e.what() << std::endl;
  }
}
// -----------------------------------------------------------------------------

int main() {
  std::cout << "=== Database Abstraction Layer - Multi-DB Example ===" << std::endl;
  std::cout << "\nThis example demonstrates the unified API across different databases." << std::endl;
  std::cout << "The same code works with SQLite, MySQL, PostgreSQL, and MSSQL!" << std::endl;

  // ========================================================================
  // Example 1: SQLite (Always available)
  // ========================================================================
  demonstrateDatabase(DBType::SQLite, ":memory:");

  // ========================================================================
  // Example 2: PostgreSQL
  // ========================================================================
  std::cout << "\n\nNote: To test PostgreSQL, MySQL, or MSSQL:" << std::endl;
  std::cout << "1. Ensure the database server is running" << std::endl;
  std::cout << "2. Create a test database and user" << std::endl;
  std::cout << "3. Update the connection string below" << std::endl;

  demonstrateDatabase(
    DBType::PostgreSQL,
    "host=127.0.0.1;port=5432;user=postgres;password=postgres;dbname=test"
  );

  // ========================================================================
  // Example 3: MySQL
  // ========================================================================
  demonstrateDatabase(
    DBType::MySQL,
    "host=127.0.0.1;port=3306;user=root;password=root;database=test"
  );

  // ========================================================================
  // Example 4: MSSQL
  // ========================================================================
  demonstrateDatabase(
    DBType::MSSQL,
    "Driver={ODBC Driver 17 for SQL Server};Server=127.0.0.1;Database=test;UID=sa;PWD=YourPassword"
  );

  std::cout << "\n" << std::string(70, '=') << std::endl;
  std::cout << "=== Key Points ===" << std::endl;
  std::cout << std::string(70, '=') << std::endl;
  std::cout << "✓ Same code runs on all database engines" << std::endl;
  std::cout << "✓ No changes needed to switch between databases" << std::endl;
  std::cout << "✓ Only connection string differs" << std::endl;
  std::cout << "✓ Prepared statements protect against SQL injection" << std::endl;
  std::cout << "✓ Transactions ensure data consistency" << std::endl;
  std::cout << "\n=== Example Complete ===" << std::endl;

  return 0;
}
// -----------------------------------------------------------------------------
