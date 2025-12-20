/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/24 13:32
  filename:  aries_base/tests/process/thread_pool/delayed.cpp

  purpose:   Basic test for delayed tasks in Aries Base ThreadPool
*********************************************************************/


// -----------------------------------------------------------------------------
#include <iostream>
#include <memory>

#include <aries_base/process/event/event.hpp>
#include <aries_base/process/thread_pool/thread_pool.hpp>
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  ThreadPool::CreateInstance(2, 8);

  Event ev(false, false);
  // post delayed task 200 ms
  ThreadPool::PostDelayedTask([](){ std::cout << "delayed run" << std::endl; }, 200, &ev);

  uint32_t event_id = ev.WaitAnyId(1000);

  if (event_id == Event::INVALID_ID) {
    std::cerr << "delayed task timeout" << std::endl;
    ThreadPool::DestroyInstance();
    return 1;
  }

  std::cout << "event triggered: " << event_id << std::endl;

  ThreadPool::DestroyInstance();
  std::cout << "delayed ok" << std::endl;
  return 0;
}
// -----------------------------------------------------------------------------
