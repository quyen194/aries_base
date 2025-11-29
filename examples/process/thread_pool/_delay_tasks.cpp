/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/23 21:57
  filename:  aries_base/examples/process/thread_pool/_delay_tasks.cpp

  purpose:   Demonstrate posting delayed tasks to the Aries Base ThreadPool
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

  // Demonstrate delayed task
  Event delayed_ev(false, false);
  ThreadPool::PostDelayedTask([]() {
    std::cout << "delayed task executed" << std::endl;
  }, /*wait_time_ms=*/200, &delayed_ev);

  if (!delayed_ev.Wait(1000)) {
    std::cerr << "timeout waiting for delayed task" << std::endl;
  }

  std::cout << "Delayed task finished\n";

  // Clean up
  ThreadPool::DestroyInstance();
  return 0;
}
// -----------------------------------------------------------------------------
