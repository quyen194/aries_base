/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/27 14:30
  filename:  aries_base/examples/process/ipc/mpmc_bounded_queue/basic.cpp

  purpose:   Basic example demonstrating IPC server and client communication
             using mpmc_bounded_queue. Shows how to create a server, connect
             a client, send messages, and properly shutdown.
*********************************************************************/


// -----------------------------------------------------------------------------
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
  std::cout << "MPMC Bounded Queue IPC - Basic Example\n";

  // Initialize thread pool
  ThreadPool::CreateInstance(2, 8);

  // Create and start server
  IpcServer server;
  server.SetName("test_ipc", false, 0);
  server.SetIpcType(IpcType::kMPSC);

  if (!server.Create(1024, 128)) {
    std::cerr << "Failed to create server\n";
    return 1;
  }

  server.CreateWorker();

  std::cout << "Server created: " << server.GetName() << "\n";

  // Create client
  IpcClient client;
  client.SetNameEx(server.GetName());

  if (!client.Connect()) {
    std::cerr << "Failed to connect client\n";
    server.Destroy();
    return 1;
  }

  std::cout << "Client connected\n";

  // Send some messages
  for (int i = 0; i < 5; ++i) {
    std::string msg = "Message " + std::to_string(i);
    if (client.Enqueue(i, msg.data(), msg.size(), 1000)) {
      std::cout << "Sent: " << msg << "\n";
    }
    std::this_thread::sleep_for(milliseconds(100));
  }

  // Give time for server to process
  std::this_thread::sleep_for(milliseconds(500));

  // Cleanup
  client.Disconnect();
  server.Destroy();

  ThreadPool::DestroyInstance();

  std::cout << "Basic example completed\n";
  return 0;
}
// -----------------------------------------------------------------------------
