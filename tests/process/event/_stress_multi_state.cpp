/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/26 05:07
  filename:  aries_base/tests/process/event/stress_multi_state.cpp

  purpose:   Stress test for multi-state events with many concurrent threads.
             Tests system stability when many threads are simultaneously
             setting and waiting on different states with high frequency.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <iostream>
#include <cassert>
#include <thread>
#include <vector>
#include <atomic>
#include <random>

#include <aries_base/process/event/event.hpp>
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace std::chrono;
using namespace aries_base::process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  std::cout << "Test: Multi-state stress test\n";

  Event ev(false, false);
  const int num_states = 20;
  const int num_setters = 10;
  const int num_waiters = 10;

  // Add states
  for (int i = 0; i < num_states; ++i) {
    ev.AddId(i * 10);
  }

  std::atomic<int> successful_waits{0};
  std::atomic<bool> stop{false};
  std::vector<std::thread> threads;

  // Setter threads
  for (int i = 0; i < num_setters; ++i) {
    threads.emplace_back([&ev, num_states, &stop, i]() {
      std::random_device rd;
      std::mt19937 gen(rd() + i);
      std::uniform_int_distribution<> dist(0, num_states - 1);

      while (!stop) {
        uint32_t state = dist(gen) * 10;
        ev.SetId(state);
        std::this_thread::sleep_for(milliseconds(10));
      }
    });
  }

  // Waiter threads (WaitAny)
  for (int i = 0; i < num_waiters; ++i) {
    threads.emplace_back([&ev, &successful_waits, &stop]() {
      while (!stop) {
        uint32_t state = ev.WaitAnyId(50);
        if (state != UINT32_MAX) {
          successful_waits++;
        }
      }
    });
  }

  // Run for 2 seconds
  std::this_thread::sleep_for(seconds(2));
  stop = true;

  for (auto& t : threads) {
    t.join();
  }

  std::cout << "Successful waits: " << successful_waits << "\n";
  assert(successful_waits > 100);  // Should have many successful waits

  std::cout << "test_event_stress_multi_state ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
