/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/29 17:20
  filename:  aries_base/examples/process/utils/exit_process_handle.cpp

  purpose:   Example usage of ExitProcessHandle utility class
*********************************************************************/


// -----------------------------------------------------------------------------
#include <iostream>
#include <thread>
#include <chrono>

#include "aries_base/process/utils/exit_process_handle.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::process::utils;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  std::cout << "aries_base ExitProcessHandle example" << std::endl;

  bool running = true;

  ExitProcessHandle exit_handle([&]() {
    std::cout << "Cleanup function called on process exit." << std::endl;
    running = false;
  });

  std::cout << "Application running... Press Ctrl+C to exit." << std::endl;

  while (running) {
    // Simulate work
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  std::cout << "ExitProcessHandle example finished" << std::endl;

  return 0;
}
// -----------------------------------------------------------------------------
