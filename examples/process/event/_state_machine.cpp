/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/26 05:13
  filename:  aries_base/examples/process/event/state_machine.cpp

  purpose:   Demonstrates using multi-state Events to implement a simple state
             machine. Shows how named states can represent different machine
             states and how WaitAny() handles state transitions.
*********************************************************************/


// -----------------------------------------------------------------------------
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

// State machine states
enum MachineState : uint32_t {
  STATE_IDLE = 100,
  STATE_RUNNING = 200,
  STATE_PAUSED = 300,
  STATE_STOPPED = 400,
  STATE_ERROR = 500
};

int main() {
  std::cout << "Event Example: State Machine\n";

  Event machine_state(false, false);
  machine_state.AddId(STATE_IDLE);
  machine_state.AddId(STATE_RUNNING);
  machine_state.AddId(STATE_PAUSED);
  machine_state.AddId(STATE_STOPPED);
  machine_state.AddId(STATE_ERROR);

  // Initial state
  machine_state.SetId(STATE_IDLE);

  // State machine controller thread
  std::thread controller([&]() {
    uint32_t current_state = machine_state.WaitAnyId(1000);
    std::cout << "Initial state: IDLE (" << current_state << ")\n";

    std::this_thread::sleep_for(milliseconds(500));
    std::cout << "Transitioning to RUNNING...\n";
    machine_state.SetId(STATE_RUNNING);

    std::this_thread::sleep_for(milliseconds(500));
    std::cout << "Transitioning to PAUSED...\n";
    machine_state.SetId(STATE_PAUSED);

    std::this_thread::sleep_for(milliseconds(500));
    std::cout << "Transitioning to RUNNING...\n";
    machine_state.SetId(STATE_RUNNING);

    std::this_thread::sleep_for(milliseconds(500));
    std::cout << "Transitioning to STOPPED...\n";
    machine_state.SetId(STATE_STOPPED);
  });

  // Monitor thread
  std::thread monitor([&]() {
    while (true) {
      uint32_t state = machine_state.WaitAnyId(2000);

      switch (state) {
        case STATE_IDLE:
          std::cout << "[Monitor] Machine is IDLE\n";
          break;
        case STATE_RUNNING:
          std::cout << "[Monitor] Machine is RUNNING\n";
          break;
        case STATE_PAUSED:
          std::cout << "[Monitor] Machine is PAUSED\n";
          break;
        case STATE_STOPPED:
          std::cout << "[Monitor] Machine STOPPED - exiting\n";
          return;
        case STATE_ERROR:
          std::cout << "[Monitor] Machine ERROR!\n";
          return;
        default:
          std::cout << "[Monitor] Unknown state or timeout\n";
          return;
      }
    }
  });

  controller.join();
  monitor.join();

  std::cout << "\nState machine example completed\n";
  return 0;
}
// -----------------------------------------------------------------------------
