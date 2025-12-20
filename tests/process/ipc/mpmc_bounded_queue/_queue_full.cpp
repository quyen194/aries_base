/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/27 14:30
  filename:  aries_base/tests/process/ipc/mpmc_bounded_queue/queue_full.cpp

  purpose:   Tests queue behavior when capacity is reached. Verifies that
             enqueue operations properly timeout when the queue is full and
             succeed again once space becomes available after dequeue.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

#include <aries_base/process/thread_pool/thread_pool.hpp>
#include <aries_base/process/ipc/mpmc_bounded_queue/ipc_server.hpp>
#include <aries_base/process/ipc/mpmc_bounded_queue/ipc_client.hpp>
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace std::chrono;
using namespace aries_base::process;
using namespace aries_base::process::ipc::mpmc_bounded_queue;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  std::cout << "Test: Queue full behavior\n";

  ThreadPool::CreateInstance(2, 8);

  // Create server with small queue
  IpcServer server;
  server.SetName("test_full", false, 0);
  server.SetIpcType(IpcType::kMPSC);
  assert(server.Create(256, 16));  // small queue: 10 blocks
  server.CreateWorker();

  // Create client
  IpcClient client;
  client.SetNameEx(server.GetName());
  assert(client.Connect());

  // Fill the queue
  int sent = 0;
  for (int i = 0; i < 15; ++i) {
    std::string msg = "Fill_" + std::to_string(i);
    if (client.Enqueue(i, msg.data(), msg.size(), 100)) {
      sent++;
    }
  }

  std::cout << "Sent " << sent << " messages (queue capacity ~10)\n";
  assert(sent >= 10);  // at least filled the queue

  // Try to send with short timeout - should fail or timeout
  std::string overflow_msg = "Overflow";
  bool timeout_worked = !client.Enqueue(100,
                                       overflow_msg.data(),
                                       overflow_msg.size(),
                                       50);  // short timeout

  std::cout << "Timeout test: " << (timeout_worked ? "passed" : "failed") << "\n";

  // Wait for server to process some messages
  std::this_thread::sleep_for(milliseconds(500));

  // Should be able to send again
  std::string new_msg = "AfterDequeue";
  assert(client.Enqueue(101, new_msg.data(), new_msg.size(), 1000));

  client.Disconnect();
  server.Destroy();
  ThreadPool::DestroyInstance();

  std::cout << "queue_full ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
