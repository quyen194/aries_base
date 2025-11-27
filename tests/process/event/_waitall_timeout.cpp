/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/26 05:07
  filename:  aries_base/tests/process/event/_waitall_timeout.cpp

  purpose:   Tests WaitAll() timeout behavior. Verifies that WaitAll() returns
             false when timeout expires before all states are signaled, and
             true only when all required states are set within timeout period.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

#include "aries_base/process/event/event.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace std::chrono;
using namespace process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  std::cout << "Test: WaitAll with timeout\n";

  // Test timeout when not all states set
  Event ev1(false, false);
  ev1.AddId(1);
  ev1.AddId(2);
  ev1.AddId(3);

  ev1.SetId(1);
  ev1.SetId(2);
  // Don't set state 3

  auto start = steady_clock::now();
  bool result = ev1.WaitAll(100);
  auto elapsed = duration_cast<milliseconds>(
      steady_clock::now() - start).count();

  assert(!result);  // Should timeout
  assert(elapsed >= 90 && elapsed <= 150);
  std::cout << "Timeout test: " << elapsed << "ms (expected ~100ms)\n";

  // Test success when all states set in time
  Event ev2(false, false);
  ev2.AddId(10);
  ev2.AddId(20);

  std::thread setter([&ev2]() {
    std::this_thread::sleep_for(milliseconds(30));
    ev2.SetId(10);
    std::this_thread::sleep_for(milliseconds(30));
    ev2.SetId(20);
  });

  start = steady_clock::now();
  result = ev2.WaitAll(200);
  elapsed = duration_cast<milliseconds>(
      steady_clock::now() - start).count();

  assert(result);  // Should succeed
  assert(elapsed >= 50 && elapsed <= 100);
  std::cout << "Success test: " << elapsed << "ms (expected ~60ms)\n";

  setter.join();

  std::cout << "test_event_waitall_timeout ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
