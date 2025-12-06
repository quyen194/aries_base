/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 07:33
  filename:  aries_base/examples/database/transaction.cpp

  purpose:   Transaction example - ACID operations
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
  std::cout << "=== Database Library - Transaction Example ===" << std::endl << std::endl;

  try {
    // Create SQLite database for this example
    auto db = DatabaseFactory::Create(DBType::SQLite);

    if (!db->Connect(":memory:")) {
      std::cerr << "Failed to connect: " << db->GetLastError() << std::endl;
      return 1;
    }

    std::cout << "Connected to SQLite database" << std::endl;

    // ====================================================================
    // Setup: Create accounts table
    // ====================================================================
    db->Execute(R"(
      CREATE TABLE accounts (
        id INTEGER PRIMARY KEY,
        name TEXT NOT NULL,
        balance REAL NOT NULL
      )
    )");

    // Insert initial accounts
    db->Execute("INSERT INTO accounts (id, name, balance) VALUES (1, 'Alice', 1000.00)");
    db->Execute("INSERT INTO accounts (id, name, balance) VALUES (2, 'Bob', 500.00)");
    db->Execute("INSERT INTO accounts (id, name, balance) VALUES (3, 'Charlie', 1500.00)");

    std::cout << "Created accounts table with initial balances:" << std::endl;
    auto result = db->Execute("SELECT id, name, balance FROM accounts ORDER BY id");
    while (result->Next()) {
      printf("  Account %d: %-10s - $%.2f\n",
           result->GetInt(0),
           result->GetString(1).c_str(),
           result->GetDouble(2));
    }
    std::cout << std::endl;

    // ====================================================================
    // Example 1: Successful Transaction
    // ====================================================================
    std::cout << "--- Example 1: Successful Transaction ---" << std::endl;
    std::cout << "Transferring $200 from Alice (ID=1) to Bob (ID=2)..." << std::endl;

    db->Begin();

    // Deduct from Alice
    auto deductResult = db->Execute("UPDATE accounts SET balance = balance - 200.00 WHERE id = 1");
    if (!deductResult) {
      std::cerr << "Failed to deduct: " << db->GetLastError() << std::endl;
      db->Rollback();
    } else {
      std::cout << "  Deducted $200 from Alice" << std::endl;
    }

    // Add to Bob
    auto addResult = db->Execute("UPDATE accounts SET balance = balance + 200.00 WHERE id = 2");
    if (!addResult) {
      std::cerr << "Failed to add: " << db->GetLastError() << std::endl;
      db->Rollback();
    } else {
      std::cout << "  Added $200 to Bob" << std::endl;
    }

    // Commit transaction
    if (db->Commit()) {
      std::cout << "Transaction committed successfully!" << std::endl;
    }

    // Show updated balances
    std::cout << "\nUpdated balances:" << std::endl;
    result = db->Execute("SELECT id, name, balance FROM accounts WHERE id IN (1, 2) ORDER BY id");
    while (result->Next()) {
      printf("  Account %d: %-10s - $%.2f\n",
           result->GetInt(0),
           result->GetString(1).c_str(),
           result->GetDouble(2));
    }
    std::cout << std::endl;

    // ====================================================================
    // Example 2: Transaction Rollback (Insufficient Funds)
    // ====================================================================
    std::cout << "--- Example 2: Transaction Rollback ---" << std::endl;
    std::cout << "Attempting to transfer $1000 from Bob (ID=2) to Charlie (ID=3)..." << std::endl;

    db->Begin();

    // Check Bob's balance first
    auto checkResult = db->Execute("SELECT balance FROM accounts WHERE id = 2");
    if (checkResult && checkResult->Next()) {
      double bobBalance = checkResult->GetDouble(0);
      std::cout << "  Bob's current balance: $" << bobBalance << std::endl;

      if (bobBalance < 1000.00) {
        std::cout << "  Insufficient funds! Rolling back transaction..." << std::endl;
        db->Rollback();
        std::cout << "Transaction rolled back successfully!" << std::endl;
      } else {
        // Would proceed with transfer
        db->Execute("UPDATE accounts SET balance = balance - 1000.00 WHERE id = 2");
        db->Execute("UPDATE accounts SET balance = balance + 1000.00 WHERE id = 3");
        db->Commit();
      }
    }

    // Verify no changes were made
    std::cout << "\nBalances after rollback (should be unchanged):" << std::endl;
    result = db->Execute("SELECT id, name, balance FROM accounts WHERE id IN (2, 3) ORDER BY id");
    while (result->Next()) {
      printf("  Account %d: %-10s - $%.2f\n",
           result->GetInt(0),
           result->GetString(1).c_str(),
           result->GetDouble(2));
    }
    std::cout << std::endl;

    // ====================================================================
    // Example 3: Complex Multi-Statement Transaction
    // ====================================================================
    std::cout << "--- Example 3: Complex Transaction ---" << std::endl;
    std::cout << "Processing multiple transfers in a single transaction..." << std::endl;

    db->Begin();

    // Transfer 1: Alice -> Bob ($100)
    db->Execute("UPDATE accounts SET balance = balance - 100.00 WHERE id = 1");
    db->Execute("UPDATE accounts SET balance = balance + 100.00 WHERE id = 2");
    std::cout << "  Transfer 1: $100 from Alice to Bob" << std::endl;

    // Transfer 2: Charlie -> Alice ($300)
    db->Execute("UPDATE accounts SET balance = balance - 300.00 WHERE id = 3");
    db->Execute("UPDATE accounts SET balance = balance + 300.00 WHERE id = 1");
    std::cout << "  Transfer 2: $300 from Charlie to Alice" << std::endl;

    // Transfer 3: Bob -> Charlie ($200)
    db->Execute("UPDATE accounts SET balance = balance - 200.00 WHERE id = 2");
    db->Execute("UPDATE accounts SET balance = balance + 200.00 WHERE id = 3");
    std::cout << "  Transfer 3: $200 from Bob to Charlie" << std::endl;

    // Commit all transfers
    if (db->Commit()) {
      std::cout << "All transfers committed successfully!" << std::endl;
    }

    // Show final balances
    std::cout << "\nFinal balances:" << std::endl;
    result = db->Execute("SELECT id, name, balance FROM accounts ORDER BY id");
    while (result->Next()) {
      printf("  Account %d: %-10s - $%.2f\n",
           result->GetInt(0),
           result->GetString(1).c_str(),
           result->GetDouble(2));
    }

    // Calculate total (should remain constant)
    result = db->Execute("SELECT SUM(balance) FROM accounts");
    if (result && result->Next()) {
      std::cout << "\nTotal money in system: $" << result->GetDouble(0)
            << " (should be $3000.00)" << std::endl;
    }

    std::cout << std::endl;

    // ====================================================================
    // Example 4: Nested Transaction Simulation
    // ====================================================================
    std::cout << "--- Example 4: Savepoint Simulation ---" << std::endl;
    std::cout << "Note: SQLite doesn't support savepoints in this simple example," << std::endl;
    std::cout << "but other databases (PostgreSQL, MSSQL) do support them." << std::endl;

    db->Begin();

    // Make some changes
    db->Execute("UPDATE accounts SET balance = balance + 50.00 WHERE id = 1");
    std::cout << "  Added $50 to Alice" << std::endl;

    // Decide to rollback
    std::cout << "  Decision: Rolling back this transaction..." << std::endl;
    db->Rollback();

    // Verify rollback
    result = db->Execute("SELECT balance FROM accounts WHERE id = 1");
    if (result && result->Next()) {
      std::cout << "  Alice's balance after rollback: $" << result->GetDouble(0)
            << " (unchanged)" << std::endl;
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
