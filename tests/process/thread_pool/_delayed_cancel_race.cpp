/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/26 05:09
  filename:  aries_base/tests/process/thread_pool/delayed_cancel_race.cpp

  purpose:   Tests race conditions in delayed task cancellation. Verifies that
             canceling a delayed task that's about to execute doesn't cause
             crashes or undefined behavior, and handles the race correctly.
*********************************************************************/


// -----------------------------------------------------------------------------
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
using namespace aries_base::process::thread_pool;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  std::cout << "Test: Delayed task cancel race condition\n";

  ThreadPool::CreateInstance(2, 8);

  // Post delayed task and cancel it right at the edge of execution
  for (int attempt = 0; attempt < 10; ++attempt) {
    Event ev(false, false);
    ev.AddId(TaskResultEvent::kCancel);
    ev.AddId(TaskResultEvent::kCancelled);
    ev.AddId(TaskResultEvent::kPass);

    ThreadPool::PostDelayedTask([attempt]() {
      std::cout << "Task " << attempt << " executed\n";
    }, 50, &ev);  // 50ms delay

    // Cancel at various times to catch different race windows
    std::this_thread::sleep_for(milliseconds(50 + (attempt % 30)));
    ev.SetId(TaskResultEvent::kCancel);

    uint32_t result = ev.WaitIds({
            TaskResultEvent::kCancelled,
            TaskResultEvent::kPass
        },
        200);
    std::cout << "Attempt " << attempt << " result: " << result << "\n";

    // Should be either PASS (executed) or FAIL (canceled)
    assert(result == TaskResultEvent::kCancelled ||
           result == TaskResultEvent::kPass);
  }

  ThreadPool::DestroyInstance();

  std::cout << "delayed_cancel_race ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
