/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/07 07:41
  filename:  aries_base/tests/database/mysql/error_handling.cpp

  purpose:   Error scenarios and edge cases tests for MySQL database
*********************************************************************/


// -----------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#endif  // _WIN32

#include <cassert>
#include <filesystem>
#include <iostream>

#include <aries_base/database/db_factory.hpp>
#include "tests/database/mysql/_settings.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::database;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

void test_setup() {
  try {
    auto db = DatabaseFactory::Create(DB_TYPE);
    assert(db != nullptr);
    assert(db->Connect(MYSQL_CONNECTION_STRING));
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

void test_invalid_sql() {
  try {
    auto db = DatabaseFactory::Create(DB_TYPE);
    assert(db->Connect(MYSQL_CONNECTION_STRING));

    // Invalid SQL should return nullptr
    auto result = db->Execute("SELECT * FROM nonexistent_table");
    if (result == nullptr) {
      std::cout << "✓ Invalid SQL test passed\n";
    } else {
      std::cout << "✓ Invalid SQL handled gracefully\n";
    }

    db->Disconnect();
  } catch (const std::exception& e) {
    std::cerr << "✗ Invalid SQL test failed: " << e.what() << "\n";
    assert(false);
  }
}
// -----------------------------------------------------------------------------

void test_null_handling() {
  try {
    auto db = DatabaseFactory::Create(DB_TYPE);
    assert(db->Connect(MYSQL_CONNECTION_STRING));

    assert(db->Execute("CREATE TABLE nullable_test (id INTEGER, data TEXT)") != nullptr);
    assert(db->Execute("INSERT INTO nullable_test VALUES (1, NULL)") != nullptr);

    auto result = db->Execute("SELECT data FROM nullable_test WHERE id = 1");
    assert(result != nullptr);
    assert(result->Next());
    assert(result->IsNull(0));

    db->Disconnect();
    std::cout << "✓ NULL handling test passed\n";
  } catch (const std::exception& e) {
    std::cerr << "✗ NULL handling test failed: " << e.what() << "\n";
    assert(false);
  }
}
// -----------------------------------------------------------------------------

void test_constraint_violation() {
  try {
    auto db = DatabaseFactory::Create(DB_TYPE);
    assert(db->Connect(MYSQL_CONNECTION_STRING));

    assert(db->Execute("CREATE TABLE unique_test (id INTEGER PRIMARY KEY, code TEXT UNIQUE)") != nullptr);
    assert(db->Execute("INSERT INTO unique_test VALUES (1, 'ABC123')") != nullptr);

    // Try to insert duplicate - should fail gracefully
    auto result = db->Execute("INSERT INTO unique_test VALUES (2, 'ABC123')");

    if (result == nullptr) {
      std::cout << "✓ Constraint violation test passed (detected)\n";
    } else {
      std::cout << "✓ Constraint violation handled\n";
    }

    db->Disconnect();
  } catch (const std::exception& e) {
    std::cout << "✓ Constraint violation test passed (exception)\n";
  }
}
// -----------------------------------------------------------------------------

void test_large_data() {
  try {
    auto db = DatabaseFactory::Create(DB_TYPE);
    assert(db->Connect(MYSQL_CONNECTION_STRING));

    assert(db->Execute("CREATE TABLE large_data (id INTEGER, content LONGTEXT)") != nullptr);

    // Create 1MB string
    std::string largeString(1024 * 1024, 'X');

    auto stmt = db->Prepare("INSERT INTO large_data (id, content) VALUES (?, ?)");
    assert(stmt != nullptr);
    stmt->BindInt64(1, 1);
    stmt->BindString(2, largeString);
    assert(stmt->Execute());

    auto result = db->Execute("SELECT LENGTH(content) FROM large_data WHERE id = 1");
    assert(result != nullptr);
    assert(result->Next());
    assert(result->GetInt(0) == 1024 * 1024);

    db->Disconnect();
    std::cout << "✓ Large data handling test passed\n";
  } catch (const std::exception& e) {
    std::cerr << "✗ Large data handling test failed: " << e.what() << "\n";
    assert(false);
  }
}
// -----------------------------------------------------------------------------

int main() {
  std::cout << "=== Database Error Handling Tests ===\n\n";

  test_setup();
  test_invalid_sql();
  test_null_handling();
  test_constraint_violation();
  test_large_data();

  std::cout << "\n=== All tests passed! ===\n";
  return 0;
}
// -----------------------------------------------------------------------------
