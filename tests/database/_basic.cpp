/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 07:38
  filename:  aries_base/tests/database/_basic.cpp

  purpose:   Basic database functionality tests using CTest
*********************************************************************/


// -----------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#endif  // _WIN32

#include <cassert>
#include <iostream>

#include <aries_base/database/db_factory.hpp>
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::database;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

void test_sqlite_connection() {
  try {
    auto db = DatabaseFactory::Create(DBType::SQLite);
    assert(db != nullptr);
    assert(db->Connect(":memory:"));
    db->Disconnect();
    std::cout << "✓ SQLite connection test passed\n";
  } catch (const std::exception& e) {
    std::cerr << "✗ SQLite connection test failed: " << e.what() << "\n";
    assert(false);
  }
}
// -----------------------------------------------------------------------------

void test_query_execution() {
  try {
    auto db = DatabaseFactory::Create(DBType::SQLite);
    assert(db->Connect(":memory:"));

    assert(db->Execute("CREATE TABLE test (id INTEGER, name TEXT)") != nullptr);
    assert(db->Execute("INSERT INTO test VALUES (1, 'test')") != nullptr);

    auto result = db->Execute("SELECT id, name FROM test");
    assert(result != nullptr);
    assert(result->Next());
    assert(result->GetInt(0) == 1);
    assert(result->GetString(1) == "test");

    db->Disconnect();
    std::cout << "✓ Query execution test passed\n";
  } catch (const std::exception& e) {
    std::cerr << "✗ Query execution test failed: " << e.what() << "\n";
    assert(false);
  }
}
// -----------------------------------------------------------------------------

void test_column_type_retrieval() {
  try {
    auto db = DatabaseFactory::Create(DBType::SQLite);
    assert(db->Connect(":memory:"));

    assert(db->Execute("CREATE TABLE types_test (id INTEGER, price REAL, name TEXT)") != nullptr);
    assert(db->Execute("INSERT INTO types_test VALUES (42, 3.14, 'item')") != nullptr);

    auto result = db->Execute("SELECT * FROM types_test");
    assert(result != nullptr && result->Next());

    assert(result->GetInt(0) == 42);
    assert(result->GetDouble(1) > 3.13 && result->GetDouble(1) < 3.15);
    assert(result->GetString(2) == "item");

    db->Disconnect();
    std::cout << "✓ Column type retrieval test passed\n";
  } catch (const std::exception& e) {
    std::cerr << "✗ Column type retrieval test failed: " << e.what() << "\n";
    assert(false);
  }
}
// -----------------------------------------------------------------------------

int main() {
  std::cout << "=== Database Basic Tests ===\n\n";

  test_sqlite_connection();
  test_query_execution();
  test_column_type_retrieval();

  std::cout << "\n=== All tests passed! ===\n";
  return 0;
}
// -----------------------------------------------------------------------------
