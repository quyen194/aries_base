/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/27 14:30
  filename:  aries_base/tests/process/ipc/mpmc_bounded_queue/client_without_server.cpp

  purpose:   Tests client behavior when server is not available. Verifies that
             client connection attempt fails gracefully when no server exists,
             and returns appropriate error codes without crashing.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cassert>
#include <cstring>
#include <iostream>

#include "aries_base/process/ipc/mpmc_bounded_queue/ipc_client.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace aries_base::process::ipc::mpmc_bounded_queue;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  std::cout << "Test: Client connection without server\n";

  IpcClient client;
  client.SetName("nonexistent_server", false, 0);

  // Try to connect to non-existent server
  bool connected = client.Connect();

  std::cout << "Connection result: " << (connected ? "success" : "failed") << "\n";
  assert(!connected);  // Should fail

  // Try to enqueue (should also fail gracefully)
  const char* msg = "Test";
  bool enqueued = client.Enqueue(1, msg, strlen(msg), 100);

  std::cout << "Enqueue result: " << (enqueued ? "success" : "failed") << "\n";
  assert(!enqueued);  // Should fail

  std::cout << "client_without_server ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
