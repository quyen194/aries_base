/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/27 14:30
  filename:  aries_base/examples/process/ipc/mpmc_bounded_queue/cross_process.cpp

  purpose:   Demonstrates cross-process IPC communication. This example shows
             how to set up a server that can be accessed by clients from other
             processes using shared memory. Run multiple instances of this
             program with "server" or "client" arguments.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <chrono>
#include <iostream>
#include <string>
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

void run_server() {
  std::cout << "Starting cross-process IPC server...\n";

  ThreadPool::CreateInstance(2, 8);

  IpcServer server;
  server.SetName("cross_proc_test", true, 12345);  // cross_process=true, port=12345
  server.SetIpcType(IpcType::kMPSC);

  if (!server.Create(2048, 128)) {
    std::cerr << "Failed to create server\n";
    return;
  }

  server.CreateWorker();

  std::cout << "Server running: " << server.GetName() << "\n";
  std::cout << "Press Ctrl+C to stop...\n";

  // Keep server running
  std::this_thread::sleep_for(seconds(60));

  server.Destroy();
  ThreadPool::DestroyInstance();
}
// -----------------------------------------------------------------------------

void run_client() {
  std::cout << "Starting cross-process IPC client...\n";

  IpcClient client;
  client.SetName("cross_proc_test", true, 12345);  // must match server

  std::cout << "Connecting to: " << "/_cross_proc_test_12345" << "\n";

  if (!client.Connect()) {
    std::cerr << "Failed to connect. Is server running?\n";
    return;
  }

  std::cout << "Client connected\n";

  // Send messages
  for (int i = 0; i < 20; ++i) {
    std::string msg = "CrossProcess_Message_" + std::to_string(i);
    if (client.Enqueue(i, msg.data(), msg.size(), 1000)) {
      std::cout << "Sent: " << msg << "\n";
    }
    std::this_thread::sleep_for(milliseconds(200));
  }

  client.Disconnect();
  std::cout << "Client disconnected\n";
}
// -----------------------------------------------------------------------------

int main(int argc, char* argv[]) {
  if (argc < 2) {
    run_server();
    std::cout << "Usage: " << argv[0] << " [server|client]\n";
    return 1;
  }

  std::string mode = argv[1];

  if (mode == "server") {
    run_server();
  } else if (mode == "client") {
    run_client();
  } else {
    std::cerr << "Invalid mode. Use 'server' or 'client'\n";
    return 1;
  }

  return 0;
}
// -----------------------------------------------------------------------------
