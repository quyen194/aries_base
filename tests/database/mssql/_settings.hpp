/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/11 16:37
  filename:  aries_base/tests/database/mssql/settings.hpp

  purpose:   MSSQL database settings for tests
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_TESTS_DATABASE_MSSQL_SETTINGS_HPP
#define ARIES_BASE_TESTS_DATABASE_MSSQL_SETTINGS_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <string>

#include <aries_base/database/db_factory.hpp>
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::database;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#define CONNECTION_STRING_MSSQL_WITHOUT_DB "Driver={ODBC Driver 17 for SQL Server};Server=127.0.0.1;UID=sa;PWD=C7A896D22505E15E7F146045E7FFA011;"
#define CONNECTION_STRING_MSSQL "Driver={ODBC Driver 17 for SQL Server};Server=127.0.0.1;UID=sa;PWD=C7A896D22505E15E7F146045E7FFA011;Database=test;"
// -----------------------------------------------------------------------------
const DBType DB_TYPE = DBType::MSSQL;
const std::string MSSQL_CONNECTION_STRING = CONNECTION_STRING_MSSQL;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_TESTS_DATABASE_MSSQL_SETTINGS_HPP
// -----------------------------------------------------------------------------
