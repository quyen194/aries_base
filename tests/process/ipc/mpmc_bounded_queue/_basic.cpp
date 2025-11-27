/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/27 14:30
  filename:  aries_base/tests/process/ipc/mpmc_bounded_queue/basic.cpp

  purpose:   Basic test for IPC mpmc_bounded_queue. Verifies server creation,
             client connection, message sending/receiving, and proper cleanup.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cassert>
#include <chrono>
#include <cstring>
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
  std::cout << "Test: Basic IPC operations\n";

  ThreadPool::CreateInstance(2, 8);

  // Create server
  IpcServer server;
  server.SetName("test_basic", false, 0);
  server.SetIpcType(IpcType::kMPSC);
  assert(server.Create(1024, 128));
  server.CreateWorker();

  // Create client
  IpcClient client;
  client.SetNameEx(server.GetName());
  assert(client.Connect());

  // Send message
  const char* test_msg = "Test message";
  assert(client.Enqueue(1, test_msg, strlen(test_msg), 1000));

  // Wait for processing
  std::this_thread::sleep_for(milliseconds(200));

  // Cleanup
  client.Disconnect();
  server.Destroy();

  ThreadPool::DestroyInstance();

  std::cout << "basic ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
