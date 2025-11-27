/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/27 14:30
  filename:  aries_base/tests/process/ipc/mpmc_bounded_queue/stress_test.cpp

  purpose:   Comprehensive stress test combining multiple concurrent clients,
             rapid message sending, varying message sizes, and long duration.
             Tests system stability and reliability under sustained heavy load.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <atomic>
#include <cassert>
#include <chrono>
#include <iostream>
#include <random>
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
  std::cout << "Test: Stress test (this may take a while)\n";

  ThreadPool::CreateInstance(8, 32);

  IpcServer server;
  server.SetName("test_stress", false, 0);
  server.SetIpcType(IpcType::kMPSC);
  assert(server.Create(2048, 512));
  server.CreateWorker();

  const int num_clients = 8;
  const int messages_per_client = 200;
  std::atomic<int> total_sent{0};
  std::atomic<int> total_failed{0};
  std::vector<std::thread> threads;

  auto start_time = steady_clock::now();

  for (int c = 0; c < num_clients; ++c) {
    threads.emplace_back([&server, c, messages_per_client, &total_sent, &total_failed]() {
      IpcClient client;
      client.SetNameEx(server.GetName());

      if (!client.Connect()) {
        std::cerr << "Client " << c << " failed to connect\n";
        return;
      }

      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<> size_dist(10, 500);

      for (int i = 0; i < messages_per_client; ++i) {
        size_t msg_size = size_dist(gen);
        std::vector<uint8_t> data(msg_size, static_cast<uint8_t>(c));

        if (client.Enqueue(c * 1000 + i, data.data(), data.size(), 500)) {
          total_sent++;
        } else {
          total_failed++;
        }
      }

      client.Disconnect();
    });
  }

  for (auto& t : threads) {
    t.join();
  }

  auto elapsed = steady_clock::now() - start_time;
  auto secs = duration_cast<seconds>(elapsed).count();

  std::cout << "Stress test completed in " << secs << " seconds\n";
  std::cout << "Total sent: " << total_sent << "\n";
  std::cout << "Total failed: " << total_failed << "\n";
  std::cout << "Success rate: " << (100.0 * total_sent / (total_sent + total_failed)) << "%\n";

  assert(total_sent > (num_clients * messages_per_client * 0.7));  // At least 70% success

  std::this_thread::sleep_for(seconds(2));

  server.Destroy();
  ThreadPool::DestroyInstance();

  std::cout << "stress_test ok\n";
  return 0;
}
// -----------------------------------------------------------------------------
