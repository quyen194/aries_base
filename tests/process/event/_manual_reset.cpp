/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/24 13:59
  filename:  aries_base/tests/process/event/_manual_reset.cpp

  purpose:   Manual-reset Event example
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

#include "aries_base/process/event/event.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace std::chrono;
using namespace process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  Event ev(true, false); // manual-reset

  const int N = 3;
  std::vector<bool> saw(N,false);
  std::vector<std::thread> threads;
  for (int i = 0; i < N; ++i) {
    threads.emplace_back([i,&ev,&saw]{
        if (ev.Wait(2000)) saw[i] = true;
    });
  }

  std::this_thread::sleep_for(milliseconds(50));
  ev.Set();

  for (auto &t: threads) t.join();
  for (int i =0;i<N;++i) assert(saw[i] && "Manual reset should wake all waiters");
  std::cout << "test_event_manual_reset: PASS\n";
  return 0;
}
// -----------------------------------------------------------------------------
