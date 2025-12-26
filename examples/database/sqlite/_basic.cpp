/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/20 06:17
  filename:  aries_base/examples/database/sqlite/basic.cpp

  purpose:   Connect and execute queries using SQLite database
*********************************************************************/


// -----------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#endif  // _WIN32

#include <iostream>
#include <filesystem>
#include <string>

#include <aries_base/database/db_factory.hpp>

#include "examples/database/sqlite/_settings.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::database;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

void setup() {
  // Remove existing test database file if any
  if (SQLITE_CONNECTION_STRING == CONNECTION_STRING_SQLITE_FILE) {
    std::filesystem::remove(CONNECTION_STRING_SQLITE_FILE);
  }
}
// -----------------------------------------------------------------------------

int main() {
  std::cout << "=== SQLite Database Library - Basic Example ===" << std::endl << std::endl;

  setup();

  try {
    auto db = DatabaseFactory::Create(DBType::SQLite);

    // Connect to database
    if (!db->Connect(SQLITE_CONNECTION_STRING)) {
      std::cerr << "Failed to connect: " << db->GetLastError() << std::endl;
      return 1;
    }

    std::cout << "Connected to database" << std::endl << std::endl;

    // Create a table
    if (!db->Execute("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT, age INTEGER)")) {
      std::cerr << "Failed to create table: " << db->GetLastError() << std::endl;
      return 1;
    }
    std::cout << "Created table 'users'" << std::endl;

    // Insert some data
    if (!db->Execute("INSERT INTO users (name, age) VALUES ('Alice', 30)")) {
      std::cerr << "Failed to insert Alice: " << db->GetLastError() << std::endl;
      return 1;
    }
    if (!db->Execute("INSERT INTO users (name, age) VALUES ('Bob', 25)")) {
      std::cerr << "Failed to insert Bob: " << db->GetLastError() << std::endl;
      return 1;
    }
    if (!db->Execute("INSERT INTO users (name, age) VALUES ('Charlie', 35)")) {
      std::cerr << "Failed to insert Charlie: " << db->GetLastError() << std::endl;
      return 1;
    }
    std::cout << "Inserted 3 users" << std::endl;

    // Query data
    auto result = db->Execute("SELECT id, name, age FROM users ORDER BY age");
    if (result) {
      std::cout << "\nQuery Results:" << std::endl;
      std::cout << "ID | Name  | Age" << std::endl;
      std::cout << "---+---------+-----" << std::endl;

      while (result->Next()) {
        int id = result->GetInt(0);
        std::string name = result->GetString(1);
        int age = result->GetInt(2);

        std::cout << id << "  | " << name;
        for (size_t i = name.length(); i < 7; ++i) std::cout << " ";
        std::cout << " | " << age << std::endl;
      }
    } else {
      std::cerr << "Failed to query users: " << db->GetLastError() << std::endl;
      return 1;
    }

    db->Disconnect();
    std::cout << "\nDisconnected from database" << std::endl;

  } catch (const std::exception& e) {
    std::cerr << "SQLite Error: " << e.what() << std::endl;
  }

  std::cout << "\n=== Example Complete ===" << std::endl;

  return 0;
}
// -----------------------------------------------------------------------------
