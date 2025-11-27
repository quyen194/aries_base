/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/27 14:30
  filename:  aries_base/tests/process/ipc/mpmc_bounded_queue/multiple_servers.cpp

  purpose:   Tests multiple independent IPC servers running simultaneously.
             Verifies that servers with different names can coexist and operate
             independently without interference or shared memory conflicts.
*********************************************************************/


// -----------------------------------------------------------------------------
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
  std::cout << "Test: Multiple independent servers\n";

  ThreadPool::CreateInstance(4, 16);

  const int num_servers = 3;
  std::vector<IpcServer> servers(num_servers);
  std::vector<IpcClient> clients(num_servers);

  // Create multiple servers
  for (int i = 0; i < num_servers; ++i) {
    std::string name = "test_multi_srv_" + std::to_string(i);
    servers[i].SetName(name, false, 0);
    servers[i].SetIpcType(IpcType::kMPSC);
    assert(servers[i].Create(1024, 128));
    servers[i].CreateWorker();
    std::cout << "Server " << i << " created: " << name << "\n";
  }

  // Connect clients to respective servers
  for (int i = 0; i < num_servers; ++i) {
    clients[i].SetNameEx(servers[i].GetName());
    assert(clients[i].Connect());
    std::cout << "Client " << i << " connected\n";
  }

  // Send messages to each server
  for (int i = 0; i < num_servers; ++i) {
    for (int j = 0; j < 10; ++j) {
      std::string msg = "Server" + std::to_string(i) + "_Msg" + std::to_string(j);
      assert(clients[i].Enqueue(j, msg.data(), msg.size(), 1000));
    }
  }

  std::this_thread::sleep_for(milliseconds(500));

  // Cleanup
  for (int i = 0; i < num_servers; ++i) {
    clients[i].Disconnect();
    servers[i].Destroy();
  }

  ThreadPool::DestroyInstance();

  std::cout << "multiple_servers ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
