/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/24 13:32
  filename:  aries_base/tests/process/thread_pool/concurrency.cpp

  purpose:   Basic test for concurrency in Aries Base ThreadPool
*********************************************************************/


// -----------------------------------------------------------------------------
#include <atomic>
#include <iostream>
#include <vector>

#include "aries_base/process/event/event.hpp"
#include "aries_base/process/thread_pool/thread_pool.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  ThreadPool::CreateInstance(4, 16);

  const int N = 100;
  std::atomic<int> counter{0};
  std::vector<std::unique_ptr<Event>> events;
  events.reserve(N);

  for (int i = 0; i < N; ++i) {
    events.emplace_back(std::make_unique<Event>(false,false));
    Event* ev = events.back().get();
    ThreadPool::PostTask([&counter, i](){
      // increment
      counter.fetch_add(1, std::memory_order_relaxed);
    }, ev);
  }

  // wait all
  for (int i = 0; i < N; ++i) {
    if (!events[i]->Wait(2000)) {
      std::cerr << "timeout waiting for task " << i << std::endl;
      ThreadPool::DestroyInstance();
      return 2;
    }
  }

  if (counter.load() != N) {
    std::cerr << "counter mismatch: " << counter.load() << " != " << N << std::endl;
    ThreadPool::DestroyInstance();
    return 3;
  }

  ThreadPool::DestroyInstance();
  std::cout << "concurrency ok" << std::endl;
  return 0;
}
// -----------------------------------------------------------------------------
