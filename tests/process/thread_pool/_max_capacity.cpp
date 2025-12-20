/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/26 05:09
  filename:  aries_base/tests/process/thread_pool/max_capacity.cpp

  purpose:   Tests ThreadPool behavior at maximum capacity. Posts more tasks
             than max_count allows and verifies that the pool handles queue
             overflow gracefully without crashes or deadlocks.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <atomic>
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

#include <aries_base/process/event/event.hpp>
#include <aries_base/process/thread_pool/thread_pool.hpp>
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace std::chrono;
using namespace aries_base::process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  std::cout << "Test: ThreadPool at max capacity\n";

  ThreadPool::CreateInstance(2, 4);  // Small pool

  std::atomic<int> executed{0};
  std::vector<std::unique_ptr<Event>> events;

  // Post many tasks (more than max workers)
  const int num_tasks = 20;
  for (int i = 0; i < num_tasks; ++i) {
    events.push_back(std::make_unique<Event>(false, false));
    ThreadPool::PostTask([&executed]() {
      executed++;
      std::this_thread::sleep_for(milliseconds(50));
    }, events.back().get());
  }

  std::cout << "Posted " << num_tasks << " tasks to pool with max=4 workers\n";

  // Wait for all with reasonable timeout
  int completed = 0;
  for (auto& ev : events) {
    if (ev->Wait(5000)) {  // Generous timeout
      completed++;
    }
  }

  std::cout << "Completed: " << completed << "/" << num_tasks << "\n";
  std::cout << "Executed: " << executed << "\n";

  assert(executed == completed);
  assert(completed >= num_tasks * 0.9);  // At least 90% should complete

  ThreadPool::DestroyInstance();

  std::cout << "max_capacity ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
