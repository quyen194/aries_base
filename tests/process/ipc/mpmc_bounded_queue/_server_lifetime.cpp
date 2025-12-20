/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/27 14:30
  filename:  aries_base/tests/process/ipc/mpmc_bounded_queue/server_lifetime.cpp

  purpose:   Tests server creation and destruction lifecycle. Verifies that
             servers can be properly created, used, destroyed, and recreated
             without resource leaks or conflicts in shared memory.
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
  std::cout << "Test: Server lifecycle\n";

  ThreadPool::CreateInstance(2, 8);

  const char* server_name = "test_lifecycle";

  // Create and destroy server multiple times
  for (int cycle = 0; cycle < 3; ++cycle) {
    std::cout << "Cycle " << cycle << "\n";

    IpcServer server;
    server.SetName(server_name, false, 0);
    server.SetIpcType(IpcType::kMPSC);
    assert(server.Create(1024, 128));
    server.CreateWorker();

    IpcClient client;
    client.SetNameEx(server.GetName());
    assert(client.Connect());

    // Send some messages
    for (int i = 0; i < 5; ++i) {
      std::string msg = "Cycle" + std::to_string(cycle) + "_Msg" + std::to_string(i);
      assert(client.Enqueue(i, msg.data(), msg.size(), 1000));
    }

    std::this_thread::sleep_for(milliseconds(200));

    client.Disconnect();
    server.Destroy();

    std::cout << "Cycle " << cycle << " completed\n";
    std::this_thread::sleep_for(milliseconds(100));
  }

  ThreadPool::DestroyInstance();

  std::cout << "server_lifetime ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
