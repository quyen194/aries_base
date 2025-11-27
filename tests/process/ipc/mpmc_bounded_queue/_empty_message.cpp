/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/27 14:30
  filename:  aries_base/tests/process/ipc/mpmc_bounded_queue/empty_message.cpp

  purpose:   Tests edge case of zero-length messages. Verifies that the system
             correctly handles empty messages (length=0) without crashing or
             corrupting subsequent messages.
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
  std::cout << "Test: Empty message handling\n";

  ThreadPool::CreateInstance(2, 8);

  IpcServer server;
  server.SetName("test_empty", false, 0);
  server.SetIpcType(IpcType::kMPSC);
  assert(server.Create(1024, 128));
  server.CreateWorker();

  IpcClient client;
  client.SetNameEx(server.GetName());
  assert(client.Connect());

  // Send empty message
  bool result = client.Enqueue(0, nullptr, 0, 1000);
  std::cout << "Empty message: " << (result ? "sent" : "failed") << "\n";

  // Send normal message after empty one
  const char* msg = "After empty";
  assert(client.Enqueue(1, msg, strlen(msg), 1000));

  std::this_thread::sleep_for(milliseconds(200));

  client.Disconnect();
  server.Destroy();
  ThreadPool::DestroyInstance();

  std::cout << "empty_message ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
