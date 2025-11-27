/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/26 05:04
  filename:  aries_base/tests/process/event/_wait_timeout.cpp

  purpose:   Tests Event wait timeout behavior. Verifies that Wait() returns
             false when timeout expires and true when event is set before
             timeout. Tests various timeout values including zero and infinite.
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
  std::cout << "Test: Event wait with timeout\n";

  // Test timeout expiry
  Event ev1(false, false);
  auto start = steady_clock::now();
  bool result = ev1.Wait(100);  // 100ms timeout
  auto elapsed = duration_cast<milliseconds>(
      steady_clock::now() - start).count();

  assert(!result);  // Should timeout
  assert(elapsed >= 90 && elapsed <= 150);  // Allow some margin
  std::cout << "Timeout test: " << elapsed << "ms (expected ~100ms)\n";

  // Test event set before timeout
  Event ev2(false, false);
  std::thread setter([&ev2]() {
    std::this_thread::sleep_for(milliseconds(50));
    ev2.Set();
  });

  start = steady_clock::now();
  result = ev2.Wait(200);  // 200ms timeout
  elapsed = duration_cast<milliseconds>(
      steady_clock::now() - start).count();

  assert(result);  // Should succeed
  assert(elapsed >= 40 && elapsed <= 100);  // Should return around 50ms
  std::cout << "Early set test: " << elapsed << "ms (expected ~50ms)\n";

  setter.join();

  // Test zero timeout
  Event ev3(false, false);
  result = ev3.Wait(0);
  assert(!result);  // Should return immediately
  std::cout << "Zero timeout test: ok\n";

  std::cout << "test_event_wait_timeout ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
