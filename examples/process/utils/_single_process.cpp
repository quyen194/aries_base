/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/29 16:25
  filename:  aries_base/examples/process/utils/single_process.cpp

  purpose:   Example usage of SingleProcess utility class
*********************************************************************/


// -----------------------------------------------------------------------------
#include <iostream>

#include "aries_base/process/utils/single_process.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::process::utils;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  std::cout << "aries_base SingleProcess example" << std::endl;

  SingleProcess single_process("SingleProcess.lock");
  if (single_process.AnotherInstanceIsRunning()) {
    // Another instance is already running
    std::cerr << "Another instance of SingleProcess is already running." << std::endl;
    return 1;
  }

  std::cout << "SingleProcess example finished" << std::endl;

  return 0;
}
// -----------------------------------------------------------------------------
