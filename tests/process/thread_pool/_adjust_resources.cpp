/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/27 15:00
  filename:  aries_base/tests/process/thread_pool/_adjust_resources.cpp

  purpose:   Tests ThreadPool dynamic resource adjustment. Verifies that
             AdjustResources() correctly modifies the idle and max thread
             counts and that the pool adapts its worker threads accordingly.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

#include "aries_base/process/event/event.hpp"
#include "aries_base/process/thread_pool/thread_pool.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace std::chrono;
using namespace process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  std::cout << "Test: ThreadPool adjust resources\n";

  ThreadPool::CreateInstance(2, 8);

  // Post some tasks
  Event ev1(false, false);
  ThreadPool::PostTask([]() {
    std::this_thread::sleep_for(milliseconds(100));
  }, &ev1);

  // Adjust resources to higher limits
  ThreadPool::AdjustResources(4, 16);
  std::cout << "Adjusted to idle=4, max=16\n";

  // Post more tasks that would benefit from more threads
  std::vector<std::unique_ptr<Event>> events;
  for (int i = 0; i < 10; ++i) {
    events.push_back(std::make_unique<Event>(false, false));
    ThreadPool::PostTask([i]() {
      std::this_thread::sleep_for(milliseconds(50));
    }, events.back().get());
  }

  // Wait for all
  for (auto& ev : events) {
    assert(ev->Wait(2000));
  }

  // Adjust down
  ThreadPool::AdjustResources(1, 4);
  std::cout << "Adjusted to idle=1, max=4\n";

  ThreadPool::DestroyInstance();

  std::cout << "adjust_resources ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
