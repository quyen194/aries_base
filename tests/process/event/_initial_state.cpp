/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/26 05:06
  filename:  aries_base/tests/process/event/_initial_state.cpp

  purpose:   Tests Event constructor initial state parameter. Verifies that
             events created with initial_state=true are immediately signaled
             and those with initial_state=false require explicit Set() call.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cassert>
#include <iostream>

#include "aries_base/process/event/event.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  std::cout << "Test: Event initial state\n";

  // Initially signaled
  Event ev_set(false, true);  // initial_state=true
  assert(ev_set.Wait(10));  // Should be immediately signaled
  std::cout << "Initial state true: ok\n";

  // Initially not signaled
  Event ev_clear(false, false);  // initial_state=false
  assert(!ev_clear.Wait(10));  // Should not be signaled
  std::cout << "Initial state false: ok\n";

  // Manual reset with initial signal
  Event ev_manual(true, true);
  assert(ev_manual.Wait(10));  // First wait
  assert(ev_manual.Wait(10));  // Still signaled (manual reset)
  std::cout << "Manual reset initial true: ok\n";

  std::cout << "test_event_initial_state ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
