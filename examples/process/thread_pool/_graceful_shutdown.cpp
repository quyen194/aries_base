/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/28 06:00
  filename:  aries_base/examples/process/thread_pool/graceful_shutdown.cpp

  purpose:   Demonstrate graceful shutdown of thread pool
*********************************************************************/


// -----------------------------------------------------------------------------
#include <chrono>
#include <iostream>
#include <thread>

#include "aries_base/process/thread_pool/thread_pool.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace std::chrono_literals;
using namespace aries_base::process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  std::cout << "=== ThreadPool graceful shutdown example ===\n";
  ThreadPool::CreateInstance(4, 16);

  for (int i = 0; i < 8; ++i) {
    ThreadPool::PostTask([i]{
      std::this_thread::sleep_for(50ms);
      std::cout << "task " << i << " done\n";
    });
  }

  std::this_thread::sleep_for(500ms);
  std::cout << "destroying pool...\n";
  ThreadPool::DestroyInstance();
  std::cout << "done.\n";
  return 0;
}
// -----------------------------------------------------------------------------
