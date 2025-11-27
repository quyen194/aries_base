/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/27 14:30
  filename:  aries_base/tests/process/ipc/mpmc_bounded_queue/sequential_ids.cpp

  purpose:   Tests message ID handling across a range of values. Verifies that
             message IDs are correctly transmitted and received, including edge
             cases like 0, max uint16_t, and sequential ranges.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cassert>
#include <chrono>
#include <cstring>
#include <iostream>
#include <limits>
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

class IdTestServer : public IpcServer {
 public:
  std::vector<uint16_t> received_ids;

 protected:
  void OnMessageReceived(uint16_t id, const uint8_t* message, uint32_t length) override {
    received_ids.push_back(id);
  }
};
// -----------------------------------------------------------------------------

int main() {
  std::cout << "Test: Message ID handling\n";

  ThreadPool::CreateInstance(2, 8);

  IdTestServer server;
  server.SetName("test_ids", false, 0);
  server.SetIpcType(IpcType::kMPSC);
  assert(server.Create(1024, 128));
  server.CreateWorker();

  IpcClient client;
  client.SetNameEx(server.GetName());
  assert(client.Connect());

  // Test various ID values
  std::vector<uint16_t> test_ids = {
      0,
      1,
      100,
      255,
      256,
      1000,
      std::numeric_limits<uint16_t>::max() - 1,
      std::numeric_limits<uint16_t>::max()
  };

  const char* msg = "ID test";
  for (uint16_t id : test_ids) {
    assert(client.Enqueue(id, msg, strlen(msg), 1000));
  }

  std::this_thread::sleep_for(milliseconds(500));

  // Verify all IDs received
  assert(server.received_ids.size() == test_ids.size());
  for (size_t i = 0; i < test_ids.size(); ++i) {
    assert(server.received_ids[i] == test_ids[i]);
    std::cout << "ID " << test_ids[i] << " ok\n";
  }

  client.Disconnect();
  server.Destroy();
  ThreadPool::DestroyInstance();

  std::cout << "sequential_ids ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
