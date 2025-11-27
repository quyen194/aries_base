/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/26 05:09
  filename:  aries_base/tests/process/thread_pool/_task_ordering.cpp

  purpose:   Tests task execution ordering in ThreadPool. Verifies that tasks
             posted in sequence are started (though not necessarily completed)
             in a reasonable order and that the queue maintains FIFO behavior.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <atomic>
#include <cassert>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "aries_base/process/event/event.hpp"
#include "aries_base/process/thread_pool/thread_pool.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  std::cout << "Test: ThreadPool task ordering\n";

  ThreadPool::CreateInstance(1, 1);  // Single thread to test ordering

  std::vector<int> execution_order;
  std::mutex order_mutex;
  std::vector<std::unique_ptr<Event>> events;

  // Post tasks in order
  for (int i = 0; i < 10; ++i) {
    events.push_back(std::make_unique<Event>(false, false));
    ThreadPool::PostTask([i, &execution_order, &order_mutex]() {
      std::lock_guard<std::mutex> lock(order_mutex);
      execution_order.push_back(i);
    }, events.back().get());
  }

  // Wait for all
  for (auto& ev : events) {
    assert(ev->Wait(2000));
  }

  // With single thread, execution order should match post order
  std::cout << "Execution order: ";
  for (int val : execution_order) {
    std::cout << val << " ";
  }
  std::cout << "\n";

  // Verify ordering
  for (size_t i = 0; i < execution_order.size(); ++i) {
    assert(execution_order[i] == static_cast<int>(i));
  }

  ThreadPool::DestroyInstance();

  std::cout << "task_ordering ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
