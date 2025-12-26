/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 07:32
  filename:  aries_base/examples/database/prepare.cpp

  purpose:   Prepared statement example - parameterized queries
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
  std::cout << "=== Database Library - Prepared Statement Example ===" << std::endl << std::endl;

  try {
    // Create SQLite database for this example
    auto db = DatabaseFactory::Create(DBType::SQLite);

    if (!db->Connect(":memory:")) {
      std::cerr << "Failed to connect: " << db->GetLastError() << std::endl;
      return 1;
    }

    std::cout << "Connected to SQLite database" << std::endl;

    // ====================================================================
    // Setup: Create table
    // ====================================================================
    db->Execute(R"(
      CREATE TABLE users (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL,
        age INTEGER,
        email TEXT,
        balance REAL
      )
    )");
    std::cout << "Created table 'users'" << std::endl << std::endl;

    // ====================================================================
    // Example 1: Insert with prepared statement
    // ====================================================================
    std::cout << "--- Example 1: Inserting Users ---" << std::endl;

    auto insertStmt = db->Prepare("INSERT INTO users (name, age, email, balance) VALUES (?, ?, ?, ?)");

    // Insert user 1
    insertStmt->BindString(1, "Alice Johnson");
    insertStmt->BindInt(2, 30);
    insertStmt->BindString(3, "alice@example.com");
    insertStmt->BindDouble(4, 1000.50);

    if (insertStmt->Execute()) {
      std::cout << "Inserted user: Alice Johnson (ID: " << db->GetLastInsertId() << ")" << std::endl;
    }

    // Reset and insert user 2
    insertStmt->Reset();
    insertStmt->BindString(1, "Bob Smith");
    insertStmt->BindInt(2, 25);
    insertStmt->BindString(3, "bob@example.com");
    insertStmt->BindDouble(4, 2500.75);

    if (insertStmt->Execute()) {
      std::cout << "Inserted user: Bob Smith (ID: " << db->GetLastInsertId() << ")" << std::endl;
    }

    // Insert user 3
    insertStmt->Reset();
    insertStmt->BindString(1, "Charlie Brown");
    insertStmt->BindInt(2, 35);
    insertStmt->BindString(3, "charlie@example.com");
    insertStmt->BindDouble(4, 500.25);

    if (insertStmt->Execute()) {
      std::cout << "Inserted user: Charlie Brown (ID: " << db->GetLastInsertId() << ")" << std::endl;
    }

    // Insert user with NULL email
    insertStmt->Reset();
    insertStmt->BindString(1, "Diana Prince");
    insertStmt->BindInt(2, 28);
    insertStmt->BindNull(3);  // NULL email
    insertStmt->BindDouble(4, 1500.00);

    if (insertStmt->Execute()) {
      std::cout << "Inserted user: Diana Prince (ID: " << db->GetLastInsertId() << ")" << std::endl;
    }

    std::cout << std::endl;

    // ====================================================================
    // Example 2: Query with prepared statement
    // ====================================================================
    std::cout << "--- Example 2: Querying Users by Age ---" << std::endl;

    auto selectStmt = db->Prepare("SELECT id, name, age, email, balance FROM users WHERE age >= ? ORDER BY age");
    selectStmt->BindInt(1, 28);

    auto result = selectStmt->Query();
    if (result) {
      std::cout << "\nUsers aged 28 or older:" << std::endl;
      std::cout << "ID | Name      | Age | Email          | Balance" << std::endl;
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
    }

    std::cout << std::endl;

    // ====================================================================
    // Example 3: Update with prepared statement
    // ====================================================================
    std::cout << "--- Example 3: Updating User Balance ---" << std::endl;

    auto updateStmt = db->Prepare("UPDATE users SET balance = balance + ? WHERE name = ?");
    updateStmt->BindDouble(1, 500.00);  // Add $500
    updateStmt->BindString(2, "Alice Johnson");

    if (updateStmt->Execute()) {
      std::cout << "Updated Alice's balance (affected rows: " << db->GetAffectedRows() << ")" << std::endl;
    }

    // Query to verify
    auto verifyResult = db->Execute("SELECT name, balance FROM users WHERE name = 'Alice Johnson'");
    if (verifyResult && verifyResult->Next()) {
      std::cout << "Alice's new balance: $" << verifyResult->GetDouble(1) << std::endl;
    }

    std::cout << std::endl;

    // ====================================================================
    // Example 4: Delete with prepared statement
    // ====================================================================
    std::cout << "--- Example 4: Deleting Users ---" << std::endl;

    auto deleteStmt = db->Prepare("DELETE FROM users WHERE age < ?");
    deleteStmt->BindInt(1, 30);

    if (deleteStmt->Execute()) {
      std::cout << "Deleted users younger than 30 (affected rows: " << db->GetAffectedRows() << ")" << std::endl;
    }

    // Show remaining users
    auto remainingResult = db->Execute("SELECT name, age FROM users ORDER BY age");
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
    if (namedResult) {
      std::cout << "\nUsers (accessed by column name):" << std::endl;

      while (namedResult->Next()) {
        // Access by column name instead of index
        int id = namedResult->GetInt("id");
        std::string name = namedResult->GetString("name");
        int age = namedResult->GetInt("age");
        double balance = namedResult->GetDouble("balance");

        std::cout << "  ID=" << id << ", Name=" << name
             << ", Age=" << age << ", Balance=$" << balance << std::endl;
      }
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
