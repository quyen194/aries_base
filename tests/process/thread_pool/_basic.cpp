/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/24 13:30
  filename:  aries_base/tests/process/thread_pool/_basic.cpp

  purpose:   Basic test for Aries Base ThreadPool
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cassert>
#include <iostream>

#include "aries_base/process/event/event.hpp"
#include "aries_base/process/thread_pool/thread_pool.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  // Create instance
  ThreadPool::CreateInstance(2, 8);

  Event ev(false, false);
  ThreadPool::PostTask([](){ /* do nothing */ }, &ev);

  // wait for completion
  if (!ev.Wait(1000)) {
    std::cerr << "Basic task did not complete in time" << std::endl;
    ThreadPool::DestroyInstance();
    return 1;
  }

  // Clean up
  ThreadPool::DestroyInstance();
  std::cout << "basic ok" << std::endl;
  return 0;
}
// -----------------------------------------------------------------------------
