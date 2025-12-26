/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/11 16:37
  filename:  aries_base/examples/database/postgres/settings.hpp

  purpose:   PostgreSQL database settings for examples
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_EXAMPLES_DATABASE_POSTGRES_SETTINGS_HPP
#define ARIES_BASE_EXAMPLES_DATABASE_POSTGRES_SETTINGS_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <string>

#include <aries_base/database/db_factory.hpp>
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::database;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#define CONNECTION_STRING_POSGRES_WITHOUT_DB "host=127.0.0.1;port=5432;user=postgres;password=eLt43ykYChcoOdpgMhkP;"
#define CONNECTION_STRING_POSGRES "host=127.0.0.1;port=5432;user=postgres;password=eLt43ykYChcoOdpgMhkP;dbname=test;"
// -----------------------------------------------------------------------------
const std::string POSTGRES_CONNECTION_STRING = CONNECTION_STRING_POSGRES;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_EXAMPLES_DATABASE_POSTGRES_SETTINGS_HPP
// -----------------------------------------------------------------------------
