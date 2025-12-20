/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/24 13:35
  filename:  aries_base/examples/process/event/single_state.cpp

  purpose:   Single-state Event example
*********************************************************************/


// -----------------------------------------------------------------------------
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include <aries_base/process/event/event.hpp>
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace std::chrono_literals;
using namespace aries_base::process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  std::cout << "=== Single-state Event example ===\n";
  Event ev(false, false); // auto-reset, initially not signaled

  std::thread worker([&]{
    std::cout << "worker: waiting for event...\n";
    ev.Wait();
    std::cout << "worker: event received, continuing...\n";
  });

  std::this_thread::sleep_for(200ms);
  std::cout << "main: setting event\n";
  ev.Set();

  worker.join();

  std::cout << "\nSingle-state Event example finished.\n";
  return 0;
}
// -----------------------------------------------------------------------------
