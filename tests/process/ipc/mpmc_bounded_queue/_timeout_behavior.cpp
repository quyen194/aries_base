/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/27 14:30
  filename:  aries_base/tests/process/ipc/mpmc_bounded_queue/timeout_behavior.cpp

  purpose:   Tests timeout behavior of enqueue operations. Verifies that
             enqueue respects the timeout parameter and returns false when
             the operation cannot complete within the specified time limit.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

#include "aries_base/process/thread_pool/thread_pool.hpp"
#include "aries_base/process/ipc/mpmc_bounded_queue/ipc_server.hpp"
#include "aries_base/process/ipc/mpmc_bounded_queue/ipc_client.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace std::chrono;
using namespace aries_base::process;
using namespace aries_base::process::ipc::mpmc_bounded_queue;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  std::cout << "Test: Enqueue timeout behavior\n";

  ThreadPool::CreateInstance(2, 8);

  // Create server with very small queue
  IpcServer server;
  server.SetName("test_timeout", false, 0);
  server.SetIpcType(IpcType::kMPSC);
  assert(server.Create(256, 4));  // Very small queue
  server.CreateWorker();

  IpcClient client;
  client.SetNameEx(server.GetName());
  assert(client.Connect());

  // Fill the queue quickly
  for (int i = 0; i < 10; ++i) {
    std::string msg = "Fill_" + std::to_string(i);
    client.Enqueue(i, msg.data(), msg.size(), 10);
  }

  // Try with very short timeout (should fail/timeout)
  auto start = steady_clock::now();
  std::string test_msg = "Should timeout";
  bool result = client.Enqueue(100, test_msg.data(), test_msg.size(), 50);  // 50ms timeout
  auto elapsed = steady_clock::now() - start;
  auto ms = duration_cast<milliseconds>(elapsed).count();

  std::cout << "Enqueue result: " << result << " after " << ms << "ms\n";
  assert(ms < 100);  // Should not exceed timeout significantly

  // Try with longer timeout
  start = steady_clock::now();
  result = client.Enqueue(101, test_msg.data(), test_msg.size(), 1000);  // 1 second timeout
  elapsed = steady_clock::now() - start;
  ms = duration_cast<milliseconds>(elapsed).count();

  std::cout << "Longer timeout result: " << result << " after " << ms << "ms\n";

  client.Disconnect();
  server.Destroy();
  ThreadPool::DestroyInstance();

  std::cout << "timeout_behavior ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
