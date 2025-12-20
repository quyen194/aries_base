/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/28 05:59
  filename:  aries_base/examples/process/event/wait_any_priority.cpp

  purpose:   Demonstrate WaitAnyName with multiple states and auto-reset
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
  std::cout << "=== Event WaitAnyName priority example ===\n";

  Event ev(false, false); // auto-reset, initially not signaled
  ev.AddName("low", false, false);
  ev.AddName("high", false, false);

  std::string observed;
  std::thread waiter([&]{
    observed = ev.WaitAnyName(1000);
    std::cout << "waiter observed: " << observed << "\n";
  });

  std::this_thread::sleep_for(100ms);
  ev.SetName("high");
  std::this_thread::sleep_for(50ms);
  ev.SetName("low");

  waiter.join();
  std::cout << "Example finished.\n";
  return 0;
}
// -----------------------------------------------------------------------------
