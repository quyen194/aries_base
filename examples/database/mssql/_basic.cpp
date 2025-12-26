/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/20 16:13
  filename:  aries_base/examples/database/mssql/basic.cpp

  purpose:   Connect and execute queries using MSSQL database
*********************************************************************/


// -----------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#endif  // _WIN32

#include <iostream>

#include <aries_base/database/db_factory.hpp>

#include "examples/database/mssql/_settings.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::database;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

void setup() {
  try {
    auto db = DatabaseFactory::Create(DBType::MSSQL);
    db->Connect(CONNECTION_STRING_MSSQL_WITHOUT_DB);
    db->Execute("IF DB_ID(N'test') IS NOT NULL DROP DATABASE test");
    db->Execute("CREATE DATABASE test");
    db->Disconnect();
    std::cout << "✓ Example setup successful\n";
  } catch (const std::exception& e) {
    std::cerr << "✗ Example setup failed: " << e.what() << "\n";
  }
}
// -----------------------------------------------------------------------------

int main() {
  std::cout << "=== MSSQL Database Library - Basic Example ===" << std::endl << std::endl;

  setup();

  try {
    auto db = DatabaseFactory::Create(DBType::MSSQL);

    if (db->Connect(MSSQL_CONNECTION_STRING)) {
      std::cout << "Connected to database" << std::endl << std::endl;

      // Test query
      auto result = db->Execute("SELECT @@VERSION");

      if (!result) {
        std::cerr << "Query failed: " << db->GetLastError() << std::endl;
        return 1;
      }

      if (result->Next()) {
        std::cout << "MSSQL version: " << result->GetString(0) << std::endl;
      }

      db->Disconnect();
      std::cout << "Disconnected from database" << std::endl;
    } else {
      std::cout << "MSSQL not available (this is expected if server is not running)" << std::endl;
    }

  } catch (const std::exception& e) {
    std::cout << "MSSQL not compiled in or not available: " << e.what() << std::endl;
  }

  std::cout << "\n=== Example Complete ===" << std::endl;

  return 0;
}
// -----------------------------------------------------------------------------
