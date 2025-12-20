/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/26 05:09
  filename:  aries_base/tests/process/thread_pool/exception_handling.cpp

  purpose:   Tests ThreadPool behavior when tasks throw exceptions. Verifies
             that exceptions in one task don't crash the thread pool or affect
             other tasks, and that the pool continues to function normally.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cassert>
#include <iostream>
#include <stdexcept>
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
  std::cout << "Test: ThreadPool exception handling\n";

  ThreadPool::CreateInstance(2, 8);
  ThreadPool::SetExceptionHandling(true);

  // Post task that throws
  Event ev1(false, false);
  ThreadPool::PostTask([]() {
    throw std::runtime_error("Test exception");
  }, &ev1);

  std::this_thread::sleep_for(milliseconds(100));

  // Post normal task after exception
  Event ev2(false, false);
  ThreadPool::PostTask([]() {
    std::cout << "Normal task after exception\n";
  }, &ev2);

  assert(ev2.Wait(1000));  // Should still work
  std::cout << "ThreadPool survived exception\n";

  // Post multiple tasks, some throw
  std::vector<std::unique_ptr<Event>> events;
  for (int i = 0; i < 5; ++i) {
    events.push_back(std::make_unique<Event>(false, false));
    ThreadPool::PostTask([i]() {
      if (i % 2 == 0) {
        throw std::runtime_error("Exception in task " + std::to_string(i));
      }
    }, events.back().get());
  }

  std::this_thread::sleep_for(milliseconds(200));

  // Pool should still accept new tasks
  Event ev_final(false, false);
  ThreadPool::PostTask([]() {
    std::cout << "Final task executed\n";
  }, &ev_final);

  assert(ev_final.Wait(1000));

  ThreadPool::DestroyInstance();

  std::cout << "exception_handling ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
