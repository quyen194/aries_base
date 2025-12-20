/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/27 14:30
  filename:  aries_base/tests/process/ipc/mpmc_bounded_queue/large_message.cpp

  purpose:   Tests handling of large messages approaching block size limits.
             Verifies that the IPC system correctly handles messages of various
             sizes including edge cases near the maximum block size.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cassert>
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
  std::cout << "Test: Large message handling\n";

  ThreadPool::CreateInstance(2, 8);

  // Create server with larger blocks
  IpcServer server;
  server.SetName("test_large", false, 0);
  server.SetIpcType(IpcType::kMPSC);
  assert(server.Create(4096, 64));  // 4KB blocks
  server.CreateWorker();

  // Create client
  IpcClient client;
  client.SetNameEx(server.GetName());
  assert(client.Connect());

  // Test various message sizes
  std::vector<uint32_t> test_sizes = {1, 10, 100, 512, 1024, 2048, 3072};

  for (uint32_t size : test_sizes) {
    std::vector<uint8_t> data(size, 0xAB);
    bool result = client.Enqueue(static_cast<uint16_t>(size), data.data(), size, 1000);
    std::cout << "Message size " << size << ": " << (result ? "sent" : "failed") << "\n";
    assert(result);
  }

  std::this_thread::sleep_for(milliseconds(500));

  client.Disconnect();
  server.Destroy();
  ThreadPool::DestroyInstance();

  std::cout << "large_message ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
