/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 07:38
  filename:  aries_base/tests/database/postgres/basic.cpp

  purpose:   Connection and basic queries tests for PostgreSQL database
*********************************************************************/


// -----------------------------------------------------------------------------
// Tests must assert in Release builds too (default CMAKE_BUILD_TYPE=Release).
#undef NDEBUG

#ifdef _WIN32
#include <windows.h>
#endif  // _WIN32

#include <cassert>
#include <filesystem>
#include <iostream>
#include <string>

#include <aries_base/database/db_factory.hpp>
#include "tests/database/postgres/_settings.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::database;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

void test_setup() {
  try {
    auto db = DatabaseFactory::Create(DB_TYPE);
    assert(db != nullptr);
    assert(db->Connect(CONNECTION_STRING_POSGRES_WITHOUT_DB));
    assert(db->Execute("DROP DATABASE IF EXISTS test") != nullptr);
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
    assert(db->Connect(POSTGRES_CONNECTION_STRING));
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
    assert(db->Connect(POSTGRES_CONNECTION_STRING));

    assert(db->Execute("DROP TABLE IF EXISTS test") != nullptr);

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
    assert(db->Connect(POSTGRES_CONNECTION_STRING));

    assert(db->Execute("DROP TABLE IF EXISTS types_test") != nullptr);

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

void test_client_encoding() {
  try {
    auto db = DatabaseFactory::Create(DB_TYPE);
    assert(db->Connect(POSTGRES_CONNECTION_STRING));

    // Connect must leave the session in UTF8
    auto result = db->Execute("SHOW client_encoding");
    assert(result != nullptr);
    assert(result->Next());
    assert(result->GetString(0) == "UTF8");

    // "Việt" as raw UTF-8 bytes: 4 characters, 6 bytes
    const std::string text = "Vi\xE1\xBB\x87t";

    assert(db->Execute("DROP TABLE IF EXISTS encoding_test") != nullptr);
    assert(db->Execute("CREATE TABLE encoding_test (name TEXT)") != nullptr);

    // statement must be destroyed before Disconnect (it holds the raw PGconn*)
    {
      auto stmt = db->Prepare("INSERT INTO encoding_test VALUES (?)");
      assert(stmt != nullptr);
      stmt->BindString(1, text);
      assert(stmt->Execute());
    }

    result = db->Execute(
        "SELECT name, length(name), octet_length(name) FROM encoding_test");
    assert(result != nullptr);
    assert(result->Next());
    assert(result->GetString(0) == text);
    assert(result->GetInt(1) == 4);
    assert(result->GetInt(2) == 6);

    db->Disconnect();
    std::cout << "✓ Client encoding test passed\n";
  } catch (const std::exception& e) {
    std::cerr << "✗ Client encoding test failed: " << e.what() << "\n";
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
  test_client_encoding();

  std::cout << "\n=== All tests passed! ===\n";
  return 0;
}
// -----------------------------------------------------------------------------
