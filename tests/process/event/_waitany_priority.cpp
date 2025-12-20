/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/26 05:07
  filename:  aries_base/tests/process/event/waitany_priority.cpp

  purpose:   Tests WaitAny() state selection priority. Verifies that when
             multiple states are signaled simultaneously, WaitAny() returns
             the lowest-indexed state and handles priority correctly.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cassert>
#include <iostream>

#include <aries_base/process/event/event.hpp>
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  std::cout << "Test: WaitAny priority when multiple states set\n";

  Event ev(false, false);
  ev.AddId(10);  // state 0
  ev.AddId(20);  // state 1
  ev.AddId(30);  // state 2

  // Set all states
  ev.SetId(10);
  ev.SetId(20);
  ev.SetId(30);

  // WaitAny should return the first one (lowest index)
  uint32_t result = ev.WaitAnyId(100);
  assert(result == 10);  // Should return first state
  std::cout << "Multiple states set, returned: " << result << " (expected 10)\n";

  // Set only middle and last
  Event ev2(false, false);
  ev2.AddId(100);
  ev2.AddId(200);
  ev2.AddId(300);

  ev2.SetId(200);
  ev2.SetId(300);

  result = ev2.WaitAnyId(100);
  assert(result == 200);  // Should return first signaled state
  std::cout << "Partial states set, returned: " << result << " (expected 200)\n";

  std::cout << "test_event_waitany_priority ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
