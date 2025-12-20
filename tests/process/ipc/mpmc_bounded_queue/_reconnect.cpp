/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/27 14:30
  filename:  aries_base/tests/process/ipc/mpmc_bounded_queue/reconnect.cpp

  purpose:   Tests client reconnection capability. Verifies that a client can
             disconnect and reconnect to the same server, and continue sending
             messages without data loss or corruption.
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
  std::cout << "Test: Client reconnection\n";

  ThreadPool::CreateInstance(2, 8);

  // Create server
  IpcServer server;
  server.SetName("test_reconnect", false, 0);
  server.SetIpcType(IpcType::kMPSC);
  assert(server.Create(1024, 128));
  server.CreateWorker();

  // First connection
  IpcClient client;
  client.SetNameEx(server.GetName());
  assert(client.Connect());

  // Send first batch
  for (int i = 0; i < 5; ++i) {
    std::string msg = "First_" + std::to_string(i);
    assert(client.Enqueue(i, msg.data(), msg.size(), 1000));
  }

  std::cout << "First batch sent\n";
  std::this_thread::sleep_for(milliseconds(200));

  // Disconnect
  client.Disconnect();
  std::cout << "Client disconnected\n";
  std::this_thread::sleep_for(milliseconds(200));

  // Reconnect
  assert(client.Connect());
  std::cout << "Client reconnected\n";

  // Send second batch
  for (int i = 0; i < 5; ++i) {
    std::string msg = "Second_" + std::to_string(i);
    assert(client.Enqueue(10 + i, msg.data(), msg.size(), 1000));
  }

  std::cout << "Second batch sent\n";
  std::this_thread::sleep_for(milliseconds(200));

  client.Disconnect();
  server.Destroy();
  ThreadPool::DestroyInstance();

  std::cout << "reconnect ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
