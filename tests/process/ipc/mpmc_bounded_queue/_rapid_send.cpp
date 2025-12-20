/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/27 14:30
  filename:  aries_base/tests/process/ipc/mpmc_bounded_queue/rapid_send.cpp

  purpose:   Tests system performance under rapid message sending without delays.
             Verifies that the lock-free queue handles high-frequency enqueue
             operations correctly and maintains stability under stress.
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
  std::cout << "Test: Rapid message sending\n";

  ThreadPool::CreateInstance(2, 8);

  IpcServer server;
  server.SetName("test_rapid", false, 0);
  server.SetIpcType(IpcType::kMPSC);
  assert(server.Create(1024, 256));
  server.CreateWorker();

  IpcClient client;
  client.SetNameEx(server.GetName());
  assert(client.Connect());

  // Send messages as fast as possible
  const int num_messages = 500;
  int sent = 0;
  auto start = steady_clock::now();

  for (int i = 0; i < num_messages; ++i) {
    std::string msg = "Rapid_" + std::to_string(i);
    if (client.Enqueue(i, msg.data(), msg.size(), 100)) {
      sent++;
    }
  }

  auto elapsed = steady_clock::now() - start;
  auto ms = duration_cast<milliseconds>(elapsed).count();

  std::cout << "Sent " << sent << "/" << num_messages << " messages in " << ms << "ms\n";
  std::cout << "Rate: " << (sent * 1000.0 / ms) << " msg/sec\n";

  assert(sent > num_messages * 0.8);  // At least 80% should succeed

  std::this_thread::sleep_for(seconds(1));

  client.Disconnect();
  server.Destroy();
  ThreadPool::DestroyInstance();

  std::cout << "rapid_send ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
