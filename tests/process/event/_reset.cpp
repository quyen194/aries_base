/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/27 15:00
  filename:  aries_base/tests/process/event/reset.cpp

  purpose:   Tests Event Reset() functionality. Verifies that Reset() clears
             the signaled state and subsequent Wait() calls block until the
             event is set again. Tests both auto-reset and manual-reset modes.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <atomic>
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "aries_base/process/event/event.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace std::chrono;
using namespace aries_base::process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  std::cout << "Test: Event Reset functionality\n";

  // Manual reset event
  Event ev(true, false);  // manual_reset=true

  ev.Set();
  assert(ev.Wait(10));  // Should be signaled

  ev.Reset();
  assert(!ev.Wait(10));  // Should not be signaled after reset
  std::cout << "Manual reset: ok\n";

  // Auto-reset event
  Event ev2(false, false);  // auto-reset
  ev2.Set();
  assert(ev2.Wait(10));  // First wait succeeds
  assert(!ev2.Wait(10));  // Auto-reset, should not be signaled
  std::cout << "Auto-reset behavior: ok\n";

  // Reset while threads are waiting
  Event ev3(true, false);
  std::atomic<int> wake_count{0};

  std::vector<std::thread> waiters;
  for (int i = 0; i < 3; ++i) {
    waiters.emplace_back([&ev3, &wake_count]() {
      if (ev3.Wait(1000)) {
        wake_count++;
      }
    });
  }

  std::this_thread::sleep_for(milliseconds(50));
  ev3.Set();
  std::this_thread::sleep_for(milliseconds(50));
  ev3.Reset();

  for (auto& t : waiters) t.join();

  assert(wake_count == 3);  // All should have woken
  std::cout << "Reset with waiting threads: " << wake_count << " threads woken\n";

  std::cout << "test_event_reset ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
