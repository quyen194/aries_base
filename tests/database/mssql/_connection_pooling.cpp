/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 07:40
  filename:  aries_base/tests/database/mssql/connection_pooling.cpp

  purpose:   Resource management and concurrency tests for MSSQL database
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

void test_single_connection_reuse() {
  try {
    auto db = DatabaseFactory::Create(DB_TYPE);
    assert(db->Connect(CONNECTION_STRING));

    assert(db->Execute("IF OBJECT_ID(N'dbo.test', N'U') IS NOT NULL DROP TABLE dbo.test") != nullptr);

    // Execute multiple operations on same connection
    assert(db->Execute("CREATE TABLE test (id INTEGER, value TEXT)") != nullptr);
    assert(db->Execute("INSERT INTO test VALUES (1, 'first')") != nullptr);
    assert(db->Execute("INSERT INTO test VALUES (2, 'second')") != nullptr);

    auto result = db->Execute("SELECT COUNT(*) FROM test");
    assert(result != nullptr);
    assert(result->Next());
    assert(result->GetInt(0) == 2);

    db->Disconnect();
    std::cout << "✓ Single connection reuse test passed\n";
  } catch (const std::exception& e) {
    std::cerr << "✗ Single connection reuse test failed: " << e.what() << "\n";
    assert(false);
  }
}
// -----------------------------------------------------------------------------

void test_statement_reset_reuse() {
  try {
    auto db = DatabaseFactory::Create(DB_TYPE);
    assert(db->Connect(CONNECTION_STRING));

    assert(db->Execute("IF OBJECT_ID(N'dbo.test', N'U') IS NOT NULL DROP TABLE dbo.test") != nullptr);

    assert(db->Execute("CREATE TABLE test (id INTEGER IDENTITY(1,1) PRIMARY KEY, value TEXT)") != nullptr);

    auto stmt = db->Prepare("INSERT INTO test (value) VALUES (?)");
    assert(stmt != nullptr);

    // Reuse statement multiple times
    for (int i = 0; i < 10; ++i) {
      stmt->Reset();
      stmt->BindString(1, "value_" + std::to_string(i));
      assert(stmt->Execute());
    }

    auto result = db->Execute("SELECT COUNT(*) FROM test");
    assert(result != nullptr);
    assert(result->Next());
    assert(result->GetInt(0) == 10);

    db->Disconnect();
    std::cout << "✓ Statement reset/reuse test passed\n";
  } catch (const std::exception& e) {
    std::cerr << "✗ Statement reset/reuse test failed: " << e.what() << "\n";
    assert(false);
  }
}
// -----------------------------------------------------------------------------

void test_batch_operation() {
  try {
    auto db = DatabaseFactory::Create(DB_TYPE);
    assert(db->Connect(CONNECTION_STRING));

    assert(db->Execute("IF OBJECT_ID(N'dbo.test', N'U') IS NOT NULL DROP TABLE dbo.test") != nullptr);

    assert(db->Execute("CREATE TABLE test (id INTEGER IDENTITY(1,1) PRIMARY KEY, value INTEGER)") != nullptr);

    auto stmt = db->Prepare("INSERT INTO test (value) VALUES (?)");
    assert(stmt != nullptr);

    // Batch insert 100 rows
    for (int i = 0; i < 100; ++i) {
      stmt->Reset();
      stmt->BindInt(1, i * 10);
      assert(stmt->Execute());
    }

    auto result = db->Execute("SELECT COUNT(*) FROM test");
    assert(result != nullptr);
    assert(result->Next());
    assert(result->GetInt(0) == 100);

    db->Disconnect();
    std::cout << "✓ Batch operation test passed\n";
  } catch (const std::exception& e) {
    std::cerr << "✗ Batch operation test failed: " << e.what() << "\n";
    assert(false);
  }
}
// -----------------------------------------------------------------------------

void test_empty_result_set() {
  try {
    auto db = DatabaseFactory::Create(DB_TYPE);
    assert(db->Connect(CONNECTION_STRING));

    assert(db->Execute("IF OBJECT_ID(N'dbo.test', N'U') IS NOT NULL DROP TABLE dbo.test") != nullptr);

    assert(db->Execute("CREATE TABLE test (id INTEGER, value TEXT)") != nullptr);

    // Query empty table
    auto result = db->Execute("SELECT * FROM test");
    assert(result != nullptr);
    assert(!result->Next());  // Should be false for empty set

    db->Disconnect();
    std::cout << "✓ Empty result set test passed\n";
  } catch (const std::exception& e) {
    std::cerr << "✗ Empty result set test failed: " << e.what() << "\n";
    assert(false);
  }
}
// -----------------------------------------------------------------------------

int main() {
  std::cout << "=== Database Connection & Resource Tests ===\n\n";

  test_setup();
  test_single_connection_reuse();
  test_statement_reset_reuse();
  test_batch_operation();
  test_empty_result_set();

  std::cout << "\n=== All tests passed! ===\n";
  return 0;
}
// -----------------------------------------------------------------------------
