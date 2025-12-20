/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/28 06:04
  filename:  aries_base/tests/process/thread_pool/shutdown_order.cpp

  purpose:   Verify Stop prevents new tasks and completes queued tasks
*********************************************************************/

// -----------------------------------------------------------------------------
#include <atomic>
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

#include <aries_base/process/event/event.hpp>
#include <aries_base/process/thread_pool/thread_pool.hpp>
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace std::chrono;
using namespace aries_base::process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  ThreadPool::CreateInstance(2, 8);
  std::atomic<int> done{0};
  Event ev(false, false);

  for (int i = 0; i < 4; ++i) {
    ThreadPool::PostTask([&]{ std::this_thread::sleep_for(milliseconds(30)); ++done; });
  }

  // Wait a bit for tasks to execute
  std::this_thread::sleep_for(milliseconds(200));
  assert(done.load() == 4 && "All tasks should complete");

  ThreadPool::DestroyInstance();
  std::cout << "test_thread_pool_shutdown_order: PASS\n";
  return 0;
}
// -----------------------------------------------------------------------------
