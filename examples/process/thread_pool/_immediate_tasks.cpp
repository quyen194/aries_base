/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/23 21:55
  filename:  aries_base/examples/process/thread_pool/_immediate_tasks.cpp

  purpose:   Demonstrate posting immediate tasks to the Aries Base ThreadPool
*********************************************************************/


// -----------------------------------------------------------------------------
#include <iostream>
#include <chrono>
#include <vector>

#include "aries_base/process/thread_pool/thread_pool.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  std::cout << "aries_base ThreadPool example\n";

  // Create global thread-pool instance (idle_count=4, max_count=16)
  ThreadPool::CreateInstance(4, 16);

  const int kTasks = 8;
  std::vector<std::unique_ptr<Event>> events;
  events.reserve(kTasks);

  // Post several tasks, each with its own Event pointer to be signalled when
  // the worker finishes the task.
  for (int i = 0; i < kTasks; ++i) {
    events.emplace_back(std::make_unique<Event>(/*manual_reset=*/false, /*initial_state=*/false));
    Event* ev = events.back().get();

    ThreadPool::PostTask([i]() {
      // Simulate work
      std::this_thread::sleep_for(std::chrono::milliseconds(50 + (i * 10)));
      std::cout << "task " << i << " executed on thread\n";
    }, ev);
  }

  // Wait for all tasks to complete
  for (int i = 0; i < kTasks; ++i) {
    // Wait up to 2 seconds for each task
    if (!events[i]->Wait(2000)) {
      std::cerr << "timeout waiting for task " << i << "\n";
    }
  }

  std::cout << "All immediate tasks finished\n";

  // Clean up
  ThreadPool::DestroyInstance();
  return 0;
}
// -----------------------------------------------------------------------------
