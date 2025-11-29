/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/28 06:05
  filename:  aries_base/tests/process/event/_manual_reset_any.cpp

  purpose:   Verify manual-reset states persist after WaitAnyName
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

#include "aries_base/process/event/event.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace std::chrono;
using namespace process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  Event ev(false, false);
  ev.AddName("m", true, false);  // manual-reset
  ev.AddName("a", false, false); // auto-reset

  ev.SetName("m");

  std::string got = ev.WaitAnyName(200);
  assert(!got.empty());

  // Manual reset bit should still be set
  bool still_set = ev.WaitName("m", 50);
  assert(still_set && "Manual-reset state should persist across waits");

  // Auto-reset test
  ev.SetName("a");
  bool first = ev.WaitName("a", 50);
  bool second = ev.WaitName("a", 50);
  assert(first && !second && "Auto-reset should clear after first wait");

  std::cout << "test_event_manual_reset_any: PASS\n";
  return 0;
}
// -----------------------------------------------------------------------------
