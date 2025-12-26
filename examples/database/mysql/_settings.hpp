/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/21 06:23
  filename:  aries_base/examples/database/mysql/settings.hpp

  purpose:   MySQL database settings for examples
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_EXAMPLES_DATABASE_MYSQL_SETTINGS_HPP
#define ARIES_BASE_EXAMPLES_DATABASE_MYSQL_SETTINGS_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <string>

#include <aries_base/database/db_factory.hpp>
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::database;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#define CONNECTION_STRING_MYSQL "host=127.0.0.1;port=3306;user=root;password=;database=test"
// -----------------------------------------------------------------------------
const std::string MYSQL_CONNECTION_STRING = CONNECTION_STRING_MYSQL;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_EXAMPLES_DATABASE_MYSQL_SETTINGS_HPP
// -----------------------------------------------------------------------------
