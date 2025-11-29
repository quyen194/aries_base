/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/26 05:12
  filename:  aries_base/examples/process/event/producer_consumer.cpp

  purpose:   Demonstrates using Events for producer-consumer synchronization.
             Shows how to coordinate data production and consumption between
             threads using event signaling.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include "aries_base/process/event/event.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace std::chrono;
using namespace aries_base::process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

int main() {
  std::cout << "Event Example: Producer-Consumer\n";

  std::queue<int> data_queue;
  std::mutex queue_mutex;
  Event data_ready(false, false);  // Signal when data is available
  Event stop_signal(false, false);  // Signal to stop consumer

  // Consumer thread
  std::thread consumer([&]() {
    while (true) {
      data_ready.Wait();

      // Check if we should stop
      {
        std::lock_guard<std::mutex> lock(queue_mutex);
        if (data_queue.empty()) {
          if (stop_signal.Wait(0)) {
            break;
          }
          continue;
        }

        int value = data_queue.front();
        data_queue.pop();
        std::cout << "Consumed: " << value << "\n";

        if (!data_queue.empty()) {
          data_ready.Set();  // More data available
        }
      }
    }
    std::cout << "Consumer stopped\n";
  });

  // Producer thread
  std::thread producer([&]() {
    for (int i = 1; i <= 10; ++i) {
      {
        std::lock_guard<std::mutex> lock(queue_mutex);
        data_queue.push(i);
        std::cout << "Produced: " << i << "\n";
      }
      data_ready.Set();
      std::this_thread::sleep_for(milliseconds(100));
    }
  });

  producer.join();

  // Wait for queue to drain
  std::this_thread::sleep_for(milliseconds(500));

  stop_signal.Set();
  data_ready.Set();  // Wake up consumer

  consumer.join();

  std::cout << "\nProducer-Consumer example completed\n";
  return 0;
}
// -----------------------------------------------------------------------------
