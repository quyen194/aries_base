/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/22 10:09
  filename:  aries_base/examples/database/mysql/prepare.cpp

  purpose:   Use prepared statements safely with MySQL database
*********************************************************************/


// -----------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#endif  // _WIN32

#include <filesystem>
#include <iostream>
#include <string>

#include <aries_base/database/db_factory.hpp>

#include "examples/database/mysql/_settings.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::database;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

void setup() {
  try {
    auto db = DatabaseFactory::Create(DBType::MySQL);
    db->Connect(MYSQL_CONNECTION_STRING);
    db->Execute("DROP DATABASE IF EXISTS test");
    db->Execute("CREATE DATABASE test");
    db->Disconnect();
    std::cout << "✓ Example setup successful\n";
  } catch (const std::exception& e) {
    std::cerr << "✗ Example setup failed: " << e.what() << "\n";
  }
}
// -----------------------------------------------------------------------------

int main() {
  std::cout << "=== MySQL Database Library - Prepared Statement Example ==="
            << std::endl
            << std::endl;

  setup();

  try {
    // Create MySQL database for this example
    auto db = DatabaseFactory::Create(DBType::MySQL);

    if (!db->Connect(MYSQL_CONNECTION_STRING)) {
      std::cerr << "Failed to connect: " << db->GetLastError() << std::endl;
      return 1;
    }

    std::cout << "Connected to database" << std::endl << std::endl;

    // ====================================================================
    // Setup: Create table
    // ====================================================================
    if (!db->Execute(R"(
          CREATE TABLE users (
            id INTEGER PRIMARY KEY AUTO_INCREMENT,
            name TEXT NOT NULL,
            age INTEGER,
            email TEXT,
            balance REAL
          )
        )")) {
      std::cerr << "Failed to create table: " << db->GetLastError() << std::endl;
      return 1;
    }

    std::cout << "Created table 'users'" << std::endl << std::endl;

    // ====================================================================
    // Example 1: Insert with prepared statement
    // ====================================================================
    std::cout << "--- Example 1: Inserting Users ---" << std::endl;

    auto insertStmt = db->Prepare("INSERT INTO users (name, age, email, balance) VALUES (?, ?, ?, ?)");

    if (!insertStmt) {
      std::cerr << "Failed to prepare insert statement: " << db->GetLastError() << std::endl;
      return 1;
    }

    // Insert user 1
    insertStmt->BindString(1, "Alice Johnson");
    insertStmt->BindInt(2, 30);
    insertStmt->BindString(3, "alice@example.com");
    insertStmt->BindDouble(4, 1000.50);

    if (!insertStmt->Execute()) {
      std::cerr << "Failed to insert user: " << insertStmt->GetLastError() << std::endl;
      return 1;
    }

    std::cout << "Inserted user: Alice Johnson (ID: " << db->GetLastInsertId() << ")" << std::endl;

    // Reset and insert user 2
    insertStmt->Reset();
    insertStmt->BindString(1, "Bob Smith");
    insertStmt->BindInt(2, 25);
    insertStmt->BindString(3, "bob@example.com");
    insertStmt->BindDouble(4, 2500.75);

    if (!insertStmt->Execute()) {
      std::cerr << "Failed to insert user: " << insertStmt->GetLastError() << std::endl;
      return 1;
    }

    std::cout << "Inserted user: Bob Smith (ID: " << db->GetLastInsertId() << ")" << std::endl;

    // Insert user 3
    insertStmt->Reset();
    insertStmt->BindString(1, "Charlie Brown");
    insertStmt->BindInt(2, 35);
    insertStmt->BindString(3, "charlie@example.com");
    insertStmt->BindDouble(4, 500.25);

    if (!insertStmt->Execute()) {
      std::cerr << "Failed to insert user: " << insertStmt->GetLastError() << std::endl;
      return 1;
    }

    std::cout << "Inserted user: Charlie Brown (ID: " << db->GetLastInsertId() << ")" << std::endl;

    // Insert user with NULL email
    insertStmt->Reset();
    insertStmt->BindString(1, "Diana Prince");
    insertStmt->BindInt(2, 28);
    insertStmt->BindNull(3);  // NULL email
    insertStmt->BindDouble(4, 1500.00);

    if (!insertStmt->Execute()) {
      std::cerr << "Failed to insert user: " << insertStmt->GetLastError() << std::endl;
      return 1;
    }

    std::cout << "Inserted user: Diana Prince (ID: " << db->GetLastInsertId() << ")" << std::endl;

    std::cout << std::endl;

    // ====================================================================
    // Example 2: Query with prepared statement
    // ====================================================================
    std::cout << "--- Example 2: Querying Users by Age ---" << std::endl;

    auto selectStmt = db->Prepare("SELECT id, name, age, email, balance FROM users WHERE age >= ? ORDER BY age");

    if (!selectStmt) {
      std::cerr << "Failed to prepare select statement: " << db->GetLastError() << std::endl;
      return 1;
    }

    selectStmt->BindInt(1, 28);

    auto result = selectStmt->Query();

    if (!result) {
      std::cerr << "Failed to execute query: " << selectStmt->GetLastError() << std::endl;
      return 1;
    }

    std::cout << "Users aged 28 or older:" << std::endl;
    std::cout << "ID | Name            | Age | Email                  | Balance" << std::endl;
    std::cout << "---+-----------------+-----+------------------------+---------" << std::endl;

    while (result->Next()) {
      int id = result->GetInt(0);
      std::string name = result->GetString(1);
      int age = result->GetInt(2);
      std::string email = result->IsNull(3) ? "(null)" : result->GetString(3);
      double balance = result->GetDouble(4);

      printf("%2d | %-15s | %3d | %-22s | $%.2f\n",
            id, name.c_str(), age, email.c_str(), balance);
    }

    std::cout << std::endl;

    // ====================================================================
    // Example 3: Update with prepared statement
    // ====================================================================
    std::cout << "--- Example 3: Updating User Balance ---" << std::endl;

    auto updateStmt = db->Prepare("UPDATE users SET balance = balance + ? WHERE name = ?");

    if (!updateStmt) {
      std::cerr << "Failed to prepare update statement: " << db->GetLastError() << std::endl;
      return 1;
    }

    updateStmt->BindDouble(1, 500.00);  // Add $500
    updateStmt->BindString(2, "Alice Johnson");

    if (updateStmt->Execute()) {
      std::cout << "Updated Alice's balance" << std::endl;
    }

    // Query to verify
    auto verifyResult = db->Execute("SELECT name, balance FROM users WHERE name = 'Alice Johnson'");

    if (!verifyResult) {
      std::cerr << "Failed to verify update: " << db->GetLastError() << std::endl;
      return 1;
    }

    if (verifyResult->Next()) {
      std::cout << "Alice's new balance: $" << verifyResult->GetDouble(1) << std::endl;
    }

    std::cout << std::endl;

    // ====================================================================
    // Example 4: Delete with prepared statement
    // ====================================================================
    std::cout << "--- Example 4: Deleting Users ---" << std::endl;

    auto deleteStmt = db->Prepare("DELETE FROM users WHERE age < ?");

    if (!deleteStmt) {
      std::cerr << "Failed to prepare delete statement: " << db->GetLastError() << std::endl;
      return 1;
    }

    deleteStmt->BindInt(1, 30);

    if (deleteStmt->Execute()) {
      std::cout << "Deleted users younger than 30" << std::endl;
    }

    // Show remaining users
    auto remainingResult = db->Execute("SELECT name, age FROM users ORDER BY age");

    if (!remainingResult) {
      std::cerr << "Failed to query remaining users: " << db->GetLastError() << std::endl;
      return 1;
    }

    std::cout << "\nRemaining users:" << std::endl;
    while (remainingResult->Next()) {
      std::cout << "  - " << remainingResult->GetString(0)
            << " (age " << remainingResult->GetInt(1) << ")" << std::endl;
    }

    std::cout << std::endl;

    // ====================================================================
    // Example 5: Query by column name
    // ====================================================================
    std::cout << "--- Example 5: Using Column Names ---" << std::endl;

    auto namedResult = db->Execute("SELECT id, name, age, balance FROM users");

    if (!namedResult) {
      std::cerr << "Failed to execute named query: " << db->GetLastError() << std::endl;
      return 1;
    }

    std::cout << "\nUsers (accessed by column name):" << std::endl;

    while (namedResult->Next()) {
      // Access by column name instead of index
      int id = namedResult->GetInt("id");
      std::string name = namedResult->GetString("name");
      int age = namedResult->GetInt("age");
      double balance = namedResult->GetDouble("balance");

      std::cout << "  ID=" << id << ", "
                << "Name=" << name << ", "
                << "Age=" << age << ", "
                << "Balance=$" << balance
                << std::endl;
    }

    db->Disconnect();
    std::cout << "\n=== Example Complete ===" << std::endl;

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
// -----------------------------------------------------------------------------
