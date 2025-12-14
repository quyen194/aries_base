/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 07:44
  filename:  aries_base/tests/database/sqlite/transaction.cpp

  purpose:   ACID transaction semantics tests for SQLite database
*********************************************************************/


// -----------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#endif  // _WIN32

#include <cassert>
#include <filesystem>
#include <iostream>

#include <aries_base/database/db_factory.hpp>
#include "tests/database/sqlite/_settings.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::database;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

void test_setup() {
  // Remove existing test database file if any
  if (DB_TYPE == DBType::SQLite && CONNECTION_STRING == CONNECTION_STRING_SQLITE_FILE) {
    std::filesystem::remove(CONNECTION_STRING_SQLITE_FILE);
  }
}
// -----------------------------------------------------------------------------

void test_begin_commit() {
  try {
    auto db = DatabaseFactory::Create(DB_TYPE);
    assert(db->Connect(CONNECTION_STRING));

    assert(db->Execute("DROP TABLE IF EXISTS accounts") != nullptr);

    assert(db->Execute("CREATE TABLE accounts (id INTEGER PRIMARY KEY, balance INTEGER)") != nullptr);
    assert(db->Execute("INSERT INTO accounts VALUES (1, 100)") != nullptr);

    assert(db->Begin());
    assert(db->Execute("UPDATE accounts SET balance = 150 WHERE id = 1") != nullptr);
    assert(db->Commit());

    auto result = db->Execute("SELECT balance FROM accounts WHERE id = 1");
    assert(result != nullptr);
    assert(result->Next());
    assert(result->GetInt(0) == 150);

    db->Disconnect();
    std::cout << "✓ Begin/commit transaction test passed\n";
  } catch (const std::exception& e) {
    std::cerr << "✗ Begin/commit transaction test failed: " << e.what() << "\n";
    assert(false);
  }
}
// -----------------------------------------------------------------------------

void test_rollback() {
  try {
    auto db = DatabaseFactory::Create(DB_TYPE);
    assert(db->Connect(CONNECTION_STRING));

    assert(db->Execute("DROP TABLE IF EXISTS accounts") != nullptr);

    assert(db->Execute("CREATE TABLE accounts (id INTEGER PRIMARY KEY, balance INTEGER)") != nullptr);
    assert(db->Execute("INSERT INTO accounts VALUES (1, 100)") != nullptr);

    assert(db->Begin());
    assert(db->Execute("UPDATE accounts SET balance = 150 WHERE id = 1") != nullptr);
    assert(db->Rollback());

    auto result = db->Execute("SELECT balance FROM accounts WHERE id = 1");
    assert(result != nullptr);
    assert(result->Next());
    assert(result->GetInt(0) == 100);  // Should be unchanged

    db->Disconnect();
    std::cout << "✓ Rollback transaction test passed\n";
  } catch (const std::exception& e) {
    std::cerr << "✗ Rollback transaction test failed: " << e.what() << "\n";
    assert(false);
  }
}
// -----------------------------------------------------------------------------

void test_multi_statement_transaction() {
  try {
    auto db = DatabaseFactory::Create(DB_TYPE);
    assert(db->Connect(CONNECTION_STRING));

    assert(db->Execute("DROP TABLE IF EXISTS accounts") != nullptr);

    assert(db->Execute("CREATE TABLE accounts (id INTEGER PRIMARY KEY, balance INTEGER)") != nullptr);
    assert(db->Execute("INSERT INTO accounts VALUES (1, 100), (2, 50)") != nullptr);

    assert(db->Begin());
    assert(db->Execute("UPDATE accounts SET balance = 80 WHERE id = 1") != nullptr);
    assert(db->Execute("UPDATE accounts SET balance = 70 WHERE id = 2") != nullptr);
    assert(db->Commit());

    auto result = db->Execute("SELECT SUM(balance) FROM accounts");
    assert(result != nullptr);
    assert(result->Next());
    assert(result->GetInt(0) == 150);

    db->Disconnect();
    std::cout << "✓ Multi-statement transaction test passed\n";
  } catch (const std::exception& e) {
    std::cerr << "✗ Multi-statement transaction test failed: " << e.what() << "\n";
    assert(false);
  }
}
// -----------------------------------------------------------------------------

int main() {
  std::cout << "=== Database Transaction Tests ===\n\n";

  test_setup();
  test_begin_commit();
  test_rollback();
  test_multi_statement_transaction();

  std::cout << "\n=== All tests passed! ===\n";
  return 0;
}
// -----------------------------------------------------------------------------
