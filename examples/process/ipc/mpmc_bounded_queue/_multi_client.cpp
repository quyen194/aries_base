/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/27 14:30
  filename:  aries_base/examples/process/ipc/mpmc_bounded_queue/multi_client.cpp

  purpose:   Demonstrates multiple clients sending messages to a single server
             concurrently. Shows the multi-producer capability of the IPC system
             with proper synchronization and message ordering.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

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
  std::cout << "MPMC Bounded Queue IPC - Multiple Clients Example\n";

  // Initialize thread pool
  ThreadPool::CreateInstance(4, 16);

  // Create and start server
  IpcServer server;
  server.SetName("multi_client_test", false, 0);
  server.SetIpcType(IpcType::kMPSC);

  if (!server.Create(1024, 256)) {
    std::cerr << "Failed to create server\n";
    return 1;
  }

  server.CreateWorker();

  std::cout << "Server created: " << server.GetName() << "\n";

  const int num_clients = 3;
  const int messages_per_client = 10;
  std::vector<std::thread> client_threads;

  // Create multiple client threads
  for (int c = 0; c < num_clients; ++c) {
    client_threads.emplace_back([&server, c, messages_per_client]() {
      IpcClient client;
      client.SetNameEx(server.GetName());

      if (!client.Connect()) {
        std::cerr << "Client " << c << " failed to connect\n";
        return;
      }

      std::cout << "Client " << c << " connected\n";

      // Send messages
      for (int i = 0; i < messages_per_client; ++i) {
        std::string msg = "Client" + std::to_string(c) + "_Msg" + std::to_string(i);
        if (client.Enqueue(c * 100 + i, msg.data(), msg.size(), 1000)) {
          std::cout << "Client " << c << " sent: " << msg << "\n";
        }
        std::this_thread::sleep_for(milliseconds(50));
      }

      client.Disconnect();
      std::cout << "Client " << c << " disconnected\n";
    });
  }

  // Wait for all clients
  for (auto& t : client_threads) {
    t.join();
  }

  // Give time for server to process remaining messages
  std::this_thread::sleep_for(seconds(1));

  // Cleanup
  server.Destroy();
  ThreadPool::DestroyInstance();

  std::cout << "Multiple clients example completed\n";
  return 0;
}
// -----------------------------------------------------------------------------
