/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/28 22:33
  filename:  aries_base/examples/utils/scope_cleanup.cpp

  purpose:   Example usage of ScopeCleanup utility class
*********************************************************************/


// -----------------------------------------------------------------------------
#include <iostream>

#include "aries_base/utils/scope_cleanup.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::utils;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  std::cout << "aries_base ScopeCleanup example" << std::endl;

  {
    ScopeCleanup cleanup([]() {
      std::cout << "ScopeCleanup: Cleanup function called on scope exit." << std::endl;
    });

    std::cout << "Inside scope." << std::endl;
  }  // cleanup function is called here

  std::cout << "ScopeCleanup example finished" << std::endl;

  return 0;
}
// -----------------------------------------------------------------------------
