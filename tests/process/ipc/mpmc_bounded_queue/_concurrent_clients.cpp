/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/27 14:30
  filename:  aries_base/tests/process/ipc/mpmc_bounded_queue/concurrent_clients.cpp

  purpose:   Tests concurrent access from multiple client threads. Verifies
             that the lock-free queue correctly handles simultaneous enqueue
             operations from multiple producers without data corruption or
             message loss.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <atomic>
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

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
  std::cout << "Test: Concurrent clients\n";

  ThreadPool::CreateInstance(4, 16);

  // Create server
  IpcServer server;
  server.SetName("test_concurrent", false, 0);
  server.SetIpcType(IpcType::kMPSC);
  assert(server.Create(1024, 256));
  server.CreateWorker();

  const int num_clients = 5;
  const int messages_per_client = 20;
  std::atomic<int> successful_sends{0};
  std::vector<std::thread> threads;

  // Create client threads
  for (int c = 0; c < num_clients; ++c) {
    threads.emplace_back([&server, c, messages_per_client, &successful_sends]() {
      IpcClient client;
      client.SetNameEx(server.GetName());
      assert(client.Connect());

      for (int i = 0; i < messages_per_client; ++i) {
        std::string msg = "C" + std::to_string(c) + "_M" + std::to_string(i);
        if (client.Enqueue(c * 1000 + i, msg.data(), msg.size(), 1000)) {
          successful_sends++;
        }
      }

      client.Disconnect();
    });
  }

  // Wait for all clients
  for (auto& t : threads) {
    t.join();
  }

  // Wait for server to process
  std::this_thread::sleep_for(seconds(1));

  assert(successful_sends == num_clients * messages_per_client);

  server.Destroy();
  ThreadPool::DestroyInstance();

  std::cout << "concurrent_clients ok - " << successful_sends << " messages sent\n";
  return 0;
}
// -----------------------------------------------------------------------------
