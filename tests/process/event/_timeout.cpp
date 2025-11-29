/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/24 14:01
  filename:  aries_base/tests/process/event/timeout.cpp

  purpose:   Basic test for timeout in Aries Base Event
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

#include "aries_base/process/event/event.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  Event ev(false, false);
  bool res = ev.Wait(100); // nothing set, should timeout
  assert(res == false && "Wait should timeout and return false");
  std::cout << "test_event_timeout: PASS\n";
  return 0;
}
// -----------------------------------------------------------------------------
