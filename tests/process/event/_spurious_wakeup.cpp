/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/28 06:05
  filename:  aries_base/tests/process/event/spurious_wakeup.cpp

  purpose:   Ensure predicate guards against spurious wakeups
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

#include <aries_base/process/event/event.hpp>
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace std::chrono;
using namespace aries_base::process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  Event ev(false, false);
  ev.AddName("go", false, false);

  bool proceeded = false;
  std::thread t([&]{
    proceeded = ev.WaitName("go", 200);
  });

  // Do not set; should time out to false
  t.join();
  assert(!proceeded && "WaitName should return false on timeout without set");
  std::cout << "test_event_spurious_wakeup: PASS\n";
  return 0;
}
// -----------------------------------------------------------------------------
