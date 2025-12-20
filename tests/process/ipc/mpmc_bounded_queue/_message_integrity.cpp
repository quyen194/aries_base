/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/27 14:30
  filename:  aries_base/tests/process/ipc/mpmc_bounded_queue/message_integrity.cpp

  purpose:   Tests message integrity and ordering. Verifies that messages are
             not corrupted during transmission and that message IDs and lengths
             are correctly preserved through the IPC channel.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <cassert>
#include <chrono>
#include <iostream>
#include <string>
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

class TestServer : public IpcServer {
 public:
  std::vector<std::pair<uint16_t, std::string>> received_messages;

 protected:
  void OnMessageReceived(uint16_t id, const uint8_t* message, uint32_t length) override {
    std::string msg(reinterpret_cast<const char*>(message), length);
    received_messages.push_back({id, msg});
    std::cout << "Received: id=" << id << " msg=" << msg << "\n";
  }
};
// -----------------------------------------------------------------------------

int main() {
  std::cout << "Test: Message integrity\n";

  ThreadPool::CreateInstance(2, 8);

  // Create custom server
  TestServer server;
  server.SetName("test_integrity", false, 0);
  server.SetIpcType(IpcType::kMPSC);
  assert(server.Create(1024, 128));
  server.CreateWorker();

  // Create client
  IpcClient client;
  client.SetNameEx(server.GetName());
  assert(client.Connect());

  // Send test messages
  std::vector<std::pair<uint16_t, std::string>> sent_messages = {
      {1, "Hello"},
      {2, "World"},
      {3, "Test Message"},
      {42, "Answer to everything"},
      {999, "Last message"}
  };

  for (const auto& [id, msg] : sent_messages) {
    assert(client.Enqueue(id, msg.data(), msg.size(), 1000));
  }

  // Wait for processing
  std::this_thread::sleep_for(seconds(1));

  // Verify all messages received
  assert(server.received_messages.size() == sent_messages.size());

  // Verify message integrity
  for (size_t i = 0; i < sent_messages.size(); ++i) {
    assert(server.received_messages[i].first == sent_messages[i].first);
    assert(server.received_messages[i].second == sent_messages[i].second);
  }

  client.Disconnect();
  server.Destroy();
  ThreadPool::DestroyInstance();

  std::cout << "message_integrity ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
