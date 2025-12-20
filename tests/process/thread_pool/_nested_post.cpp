/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/26 05:10
  filename:  aries_base/tests/process/thread_pool/nested_post.cpp

  purpose:   Tests posting tasks from within running tasks (nested posting).
             Verifies that the ThreadPool can handle tasks that post new tasks
             without deadlock or resource exhaustion.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <atomic>
#include <cassert>
#include <iostream>

#include <aries_base/process/event/event.hpp>
#include <aries_base/process/thread_pool/thread_pool.hpp>
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  std::cout << "Test: Nested task posting\n";

  ThreadPool::CreateInstance(4, 16);

  std::atomic<int> total_executed{0};
  Event final_event(false, false);

  // Post a task that posts more tasks
  ThreadPool::PostTask([&total_executed, &final_event]() {
    total_executed++;
    std::cout << "Level 1 task\n";

    // Post child tasks
    std::vector<std::unique_ptr<Event>> child_events;
    for (int i = 0; i < 5; ++i) {
      child_events.push_back(std::make_unique<Event>(false, false));
      ThreadPool::PostTask([&total_executed, i]() {
        total_executed++;
        std::cout << "Level 2 task " << i << "\n";
      }, child_events.back().get());
    }

    // Wait for children
    for (auto& ev : child_events) {
      ev->Wait(1000);
    }

    final_event.Set();
  }, nullptr);

  assert(final_event.Wait(5000));
  std::cout << "Total executed: " << total_executed << " (expected 6)\n";
  assert(total_executed == 6);

  ThreadPool::DestroyInstance();

  std::cout << "nested_post ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
