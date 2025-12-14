/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 07:38
  filename:  aries_base/tests/database/mssql/basic.cpp

  purpose:   Connection and basic queries tests for MSSQL database
*********************************************************************/


// -----------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#endif  // _WIN32

#include <cassert>
#include <iostream>
#include <string>

#include <aries_base/database/db_factory.hpp>
#include "tests/database/mssql/_settings.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::database;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

void test_setup() {
  try {
    auto db = DatabaseFactory::Create(DB_TYPE);
    assert(db != nullptr);
    assert(db->Connect(CONNECTION_STRING_MSSQL_WITHOUT_DB));
    assert(db->Execute("IF DB_ID(N'test') IS NOT NULL DROP DATABASE test") != nullptr);
    assert(db->Execute("CREATE DATABASE test") != nullptr);
    db->Disconnect();
    std::cout << "✓ Pre-test setup successful\n";
  } catch (const std::exception& e) {
    std::cerr << "✗ Pre-test setup failed: " << e.what() << "\n";
    assert(false);
  }
}
// -----------------------------------------------------------------------------

void test_sql_connection() {
  try {
    auto db = DatabaseFactory::Create(DB_TYPE);
    assert(db != nullptr);
    assert(db->Connect(CONNECTION_STRING));
    db->Disconnect();
    std::cout << "✓ Connection test passed\n";
  } catch (const std::exception& e) {
    std::cerr << "✗ Connection test failed: " << e.what() << "\n";
    assert(false);
  }
}
// -----------------------------------------------------------------------------

void test_query_execution() {
  try {
    auto db = DatabaseFactory::Create(DB_TYPE);
    assert(db->Connect(CONNECTION_STRING));

    assert(db->Execute("IF OBJECT_ID(N'dbo.test', N'U') IS NOT NULL DROP TABLE dbo.test") != nullptr);

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
    auto db = DatabaseFactory::Create(DB_TYPE);
    assert(db->Connect(CONNECTION_STRING));

    assert(db->Execute("IF OBJECT_ID(N'dbo.test', N'U') IS NOT NULL DROP TABLE dbo.test") != nullptr);

    assert(db->Execute("CREATE TABLE types_test (id INTEGER, price REAL, name TEXT)") != nullptr);
    assert(db->Execute("INSERT INTO types_test VALUES (42, 3.14, 'item')") != nullptr);

    auto result = db->Execute("SELECT * FROM types_test");
    assert(result != nullptr);
    assert(result->Next());

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

  test_setup();
  test_sql_connection();
  test_query_execution();
  test_column_type_retrieval();

  std::cout << "\n=== All tests passed! ===\n";
  return 0;
}
// -----------------------------------------------------------------------------
