/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/24 13:35
  filename:  libs/aries_base/examples/process/event/multi_state.cpp

  purpose:   Multi-state Event example
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
using namespace process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  std::cout << "\n=== Multi-state Event example ===\n";
  Event ev; // single-state by default

  // Add named states
  ev.Add("one", false, false);
  ev.Add("two", false, false);

  // WaitAny example
  std::thread waiter_any([&]{
    std::cout << "waiter_any: waiting for either 'one' or 'two'...\n";
    std::string s = ev.WaitAny();
    std::cout << "waiter_any: signaled state: " << s << "\n";
  });

  // WaitAll example
  std::thread waiter_all([&]{
    std::cout << "waiter_all: waiting for both 'one' and 'two'...\n";
    bool ok = ev.WaitAll(5000); // 5s timeout
    std::cout << "waiter_all: WaitAll returned " << (ok ? "true" : "false") << "\n";
  });

  std::this_thread::sleep_for(200ms);
  std::cout << "main: set 'one'\n";
  ev.Set("one");

  std::this_thread::sleep_for(200ms);
  std::cout << "main: set 'two'\n";
  ev.Set("two");

  waiter_any.join();
  waiter_all.join();

  std::cout << "\nMulti-state Event example finished.\n";
  return 0;
}
// -----------------------------------------------------------------------------
