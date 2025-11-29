/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/24 14:01
  filename:  aries_base/tests/process/event/single.cpp

  purpose:   Basic test for single-state Event in Aries Base
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

#include "aries_base/process/event/event.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  Event ev(false, false); // auto-reset, initially not signaled

  bool signaled = false;

  std::thread t([&]{
    // wait in other thread
    if (ev.Wait(1000)) {
      signaled = true;
    }
  });

  // ensure waiter is waiting
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  ev.Set();

  t.join();
  assert(signaled && "Single-state waiter should be signaled");
  std::cout << "test_event_single: PASS\n";
  return 0;
}
// -----------------------------------------------------------------------------
