/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/24 13:31
  filename:  aries_base/tests/process/thread_pool/_cancel.cpp

  purpose:   Basic test for cancelling tasks in Aries Base ThreadPool
*********************************************************************/


// -----------------------------------------------------------------------------
#include <iostream>
#include <memory>

#include "aries_base/process/event/event.hpp"
#include "aries_base/process/thread_pool/thread_pool.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace process;
using namespace process::thread_pool;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  ThreadPool::CreateInstance(2, 8);

  Event ev(false, false);
  Event cancel_ev(false, false);

  // Post delayed task far in future and cancel it
  ThreadPool::PostDelayedTask([](){ std::cout << "should not run" << std::endl; }, 5000, &ev);

  // The ThreadPool's PostDelayedTask attaches named end-state flags to the
  // provided `Event` (it calls `task_end_events->Add(TASK_CANCEL_EVENT, ...)`,
  // etc.). To cancel a delayed task the caller sets the cancel named state
  // on the same `Event` object. The Event API exposes `Set(const std::string&)`
  // so callers can do `ev.Set(ThreadPool::TASK_CANCEL_EVENT)`.

  // Try to cancel immediately by setting the named cancel state.
  ev.Set(ThreadPool::TASK_CANCEL_EVENT);

  // The ThreadPool will observe the cancel flag and mark the task as failed by
  // setting `TASK_FAIL_EVENT` on the same `Event`. Wait for that as the
  // canonical confirmation of cancellation (best-effort with timeout).
  if (!ev.Wait(ThreadPool::TASK_FAIL_EVENT, 1000)) {
    std::cerr << "cancellation test: did not observe TASK_FAIL_EVENT within timeout" << std::endl;
    ThreadPool::DestroyInstance();
    return 2;
  }

  ThreadPool::DestroyInstance();
  std::cout << "cancel check done (best-effort)" << std::endl;
  return 0;
}
// -----------------------------------------------------------------------------
