/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/28 05:59
  filename:  aries_base/examples/process/event/wait_all_auto_reset.cpp

  purpose:   Demonstrate WaitAll with auto-reset states
*********************************************************************/


// -----------------------------------------------------------------------------
#include <chrono>
#include <iostream>
#include <thread>

#include "aries_base/process/event/event.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace std::chrono_literals;
using namespace aries_base::process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  std::cout << "=== Event WaitAll auto-reset example ===\n";

  Event ev(false, false); // auto-reset
  ev.AddName("A", false, false);
  ev.AddName("B", false, false);

  bool done = false;
  std::thread waiter([&]{
    done = ev.WaitAll(1000);
    std::cout << "WaitAll returned: " << (done ? "true" : "false") << "\n";
  });

  std::this_thread::sleep_for(100ms);
  ev.SetName("A");
  std::this_thread::sleep_for(100ms);
  ev.SetName("B");

  waiter.join();
  std::cout << "Example finished.\n";
  return 0;
}
// -----------------------------------------------------------------------------
