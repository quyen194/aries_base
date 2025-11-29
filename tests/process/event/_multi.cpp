/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/24 14:00
  filename:  aries_base/tests/process/event/_multi.cpp

  purpose:   Basic test for multi-state Event in Aries Base
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

#include "aries_base/process/event/event.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  Event ev; // single-state default but we'll use named states
  ev.Add("a", false, false);
  ev.Add("b", false, false);

  bool waiter_any_ok = false;
  std::thread t_any([&]{
    std::string s = ev.WaitAny(1000);
    if (!s.empty()) waiter_any_ok = true;
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  ev.Set("b");

  t_any.join();
  assert(waiter_any_ok && "WaitAny should return when a state is set");

  // WaitAll case
  bool wait_all_ok = false;
  std::thread t_all([&]{
    wait_all_ok = ev.WaitAll(1000);
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  ev.Set("a");
  ev.Set("b");

  t_all.join();
  assert(wait_all_ok && "WaitAll should return true when all states set");

  std::cout << "test_event_multi: PASS\n";
  return 0;
}
// -----------------------------------------------------------------------------
