/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 07:43
  filename:  aries_base/tests/database/sqlite/prepared_statement.cpp

  purpose:   Parameter binding and statement reuse for SQLite database
*********************************************************************/


// -----------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#endif  // _WIN32

#include <cassert>
#include <filesystem>
#include <iostream>
#include <string>

#include <aries_base/database/db_factory.hpp>
#include "tests/database/sqlite/_settings.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::database;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

void test_setup() {
  // Remove existing test database file if any
  if (DB_TYPE == DBType::SQLite && SQLITE_CONNECTION_STRING == CONNECTION_STRING_SQLITE_FILE) {
    std::filesystem::remove(CONNECTION_STRING_SQLITE_FILE);
  }
}
// -----------------------------------------------------------------------------

void test_bind_integer() {
  try {
    auto db = DatabaseFactory::Create(DB_TYPE);
    assert(db->Connect(SQLITE_CONNECTION_STRING));

    assert(db->Execute("DROP TABLE IF EXISTS test") != nullptr);

    assert(db->Execute("CREATE TABLE test (id INTEGER PRIMARY KEY, value INTEGER)") != nullptr);

    auto stmt = db->Prepare("INSERT INTO test (value) VALUES (?)");
    assert(stmt != nullptr);
    stmt->BindInt(1, 42);
    assert(stmt->Execute());

    auto result = db->Execute("SELECT value FROM test WHERE id = 1");
    assert(result != nullptr);
    assert(result->Next());
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
    auto db = DatabaseFactory::Create(DB_TYPE);
    assert(db->Connect(SQLITE_CONNECTION_STRING));

    assert(db->Execute("DROP TABLE IF EXISTS test") != nullptr);

    assert(db->Execute("CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT)") != nullptr);

    auto stmt = db->Prepare("INSERT INTO test (value) VALUES (?)");
    assert(stmt != nullptr);
    stmt->BindString(1, "TestData");
    assert(stmt->Execute());

    auto result = db->Execute("SELECT value FROM test WHERE id = 1");
    assert(result != nullptr);
    assert(result->Next());
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
    auto db = DatabaseFactory::Create(DB_TYPE);
    assert(db->Connect(SQLITE_CONNECTION_STRING));

    assert(db->Execute("DROP TABLE IF EXISTS test") != nullptr);

    assert(db->Execute("CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT)") != nullptr);

    auto stmt = db->Prepare("INSERT INTO test (value) VALUES (?)");
    assert(stmt != nullptr);

    for (int i = 0; i < 5; ++i) {
      stmt->Reset();
      stmt->BindString(1, "item_" + std::to_string(i));
      assert(stmt->Execute());
    }

    auto result = db->Execute("SELECT COUNT(*) FROM test");
    assert(result != nullptr);
    assert(result->Next());
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

  test_setup();
  test_bind_integer();
  test_bind_string();
  test_batch_insert();

  std::cout << "\n=== All tests passed! ===\n";
  return 0;
}
// -----------------------------------------------------------------------------
