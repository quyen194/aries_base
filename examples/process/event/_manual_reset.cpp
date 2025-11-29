/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/24 13:35
  filename:  aries_base/examples/process/event/manual_reset.cpp

  purpose:   Manual-reset Event example
*********************************************************************/


// -----------------------------------------------------------------------------
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "aries_base/process/event/event.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace std::chrono_literals;
using namespace aries_base::process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  std::cout << "\n=== Manual-reset Event example ===\n";
  Event ev(true, false); // manual-reset, initially not signaled

  std::vector<std::thread> workers;
  for (int i = 0; i < 3; ++i) {
    workers.emplace_back([i, &ev]{
      std::cout << "worker" << i << ": waiting...\n";
      ev.Wait();
      std::cout << "worker" << i << ": woke up (manual-reset)\n";
    });
  }

  std::this_thread::sleep_for(200ms);
  std::cout << "main: setting manual-reset event (all waiters should run)\n";
  ev.Set();

  std::this_thread::sleep_for(100ms);
  std::cout << "main: resetting event\n";
  ev.Reset();

  for (auto &t : workers) t.join();

  std::cout << "\nManual-reset Event example finished.\n";
  return 0;
}
// -----------------------------------------------------------------------------
