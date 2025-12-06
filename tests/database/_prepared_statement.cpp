/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 07:43
  filename:  aries_base/tests/database/_prepared_statement.cpp

  purpose:   Prepared statement tests using CTest
*********************************************************************/


// -----------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#endif  // _WIN32

#include <cassert>
#include <iostream>
#include <string>

#include <aries_base/database/db_factory.hpp>
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::database;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

void test_bind_integer() {
  try {
    auto db = DatabaseFactory::Create(DBType::SQLite);
    assert(db->Connect(":memory:"));

    assert(db->Execute("CREATE TABLE test (id INTEGER PRIMARY KEY, value INTEGER)") != nullptr);

    auto stmt = db->Prepare("INSERT INTO test (value) VALUES (?)");
    assert(stmt != nullptr);
    stmt->BindInt(1, 42);
    assert(stmt->Execute());

    auto result = db->Execute("SELECT value FROM test WHERE id = 1");
    assert(result != nullptr && result->Next());
    assert(result->GetInt(0) == 42);

    db->Disconnect();
    std::cout << "✓ Bind integer test passed\n";
  } catch (const std::exception& e) {
    std::cerr << "✗ Bind integer test failed: " << e.what() << "\n";
    assert(false);
  }
}
// -----------------------------------------------------------------------------

void test_bind_string() {
  try {
    auto db = DatabaseFactory::Create(DBType::SQLite);
    assert(db->Connect(":memory:"));

    assert(db->Execute("CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT)") != nullptr);

    auto stmt = db->Prepare("INSERT INTO test (value) VALUES (?)");
    assert(stmt != nullptr);
    stmt->BindString(1, "TestData");
    assert(stmt->Execute());

    auto result = db->Execute("SELECT value FROM test WHERE id = 1");
    assert(result != nullptr && result->Next());
    assert(result->GetString(0) == "TestData");

    db->Disconnect();
    std::cout << "✓ Bind string test passed\n";
  } catch (const std::exception& e) {
    std::cerr << "✗ Bind string test failed: " << e.what() << "\n";
    assert(false);
  }
}
// -----------------------------------------------------------------------------

void test_batch_insert() {
  try {
    auto db = DatabaseFactory::Create(DBType::SQLite);
    assert(db->Connect(":memory:"));

    assert(db->Execute("CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT)") != nullptr);

    auto stmt = db->Prepare("INSERT INTO test (value) VALUES (?)");
    assert(stmt != nullptr);

    for (int i = 0; i < 5; ++i) {
      stmt->Reset();
      stmt->BindString(1, "item_" + std::to_string(i));
      assert(stmt->Execute());
    }

    auto result = db->Execute("SELECT COUNT(*) FROM test");
    assert(result != nullptr && result->Next());
    assert(result->GetInt(0) == 5);

    db->Disconnect();
    std::cout << "✓ Batch insert test passed\n";
  } catch (const std::exception& e) {
    std::cerr << "✗ Batch insert test failed: " << e.what() << "\n";
    assert(false);
  }
}
// -----------------------------------------------------------------------------

int main() {
  std::cout << "=== Database Prepared Statement Tests ===\n\n";

  test_bind_integer();
  test_bind_string();
  test_batch_insert();

  std::cout << "\n=== All tests passed! ===\n";
  return 0;
}
// -----------------------------------------------------------------------------
