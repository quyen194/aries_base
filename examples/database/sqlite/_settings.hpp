/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/20 06:01
  filename:  aries_base/examples/database/sqlite/settings.hpp

  purpose:   SQLite database settings for examples
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_EXAMPLES_DATABASE_SQLITE_SETTINGS_HPP
#define ARIES_BASE_EXAMPLES_DATABASE_SQLITE_SETTINGS_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <string>

#include <aries_base/database/db_factory.hpp>
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::database;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#define CONNECTION_STRING_SQLITE_MEMORY ":memory:"
#define CONNECTION_STRING_SQLITE_FILE "test.db"
// -----------------------------------------------------------------------------
const std::string SQLITE_CONNECTION_STRING = CONNECTION_STRING_SQLITE_FILE;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_EXAMPLES_DATABASE_SQLITE_SETTINGS_HPP
// -----------------------------------------------------------------------------
