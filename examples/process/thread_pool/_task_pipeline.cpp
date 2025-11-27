/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/26 05:15
  filename:  aries_base/examples/process/thread_pool/_task_pipeline.cpp

  purpose:   Demonstrates creating a task pipeline where tasks are chained and
             each stage depends on the previous. Shows how to coordinate
             multi-stage processing using Events and ThreadPool.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <vector>

#include "aries_base/process/event/event.hpp"
#include "aries_base/process/thread_pool/thread_pool.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace std::chrono;
using namespace process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

struct DataItem {
  int id;
  std::string data;
  int processed_stage = 0;
};

int main() {
  std::cout << "ThreadPool Example: Task Pipeline\n";

  ThreadPool::CreateInstance(4, 16);

  std::vector<DataItem> items;
  for (int i = 0; i < 5; ++i) {
    items.push_back({i, "Item_" + std::to_string(i), 0});
  }

  // Stage 1: Load
  std::vector<std::unique_ptr<Event>> stage1_events;
  for (auto& item : items) {
    stage1_events.push_back(std::make_unique<Event>(false, false));
    ThreadPool::PostTask([&item]() {
      std::cout << "[Stage 1] Loading item " << item.id << "\n";
      std::this_thread::sleep_for(milliseconds(100));
      item.data += "_loaded";
      item.processed_stage = 1;
    }, stage1_events.back().get());
  }

  // Wait for stage 1
  for (auto& ev : stage1_events) {
    ev->Wait(2000);
  }
  std::cout << "Stage 1 completed\n\n";

  // Stage 2: Process
  std::vector<std::unique_ptr<Event>> stage2_events;
  for (auto& item : items) {
    stage2_events.push_back(std::make_unique<Event>(false, false));
    ThreadPool::PostTask([&item]() {
      std::cout << "[Stage 2] Processing item " << item.id << "\n";
      std::this_thread::sleep_for(milliseconds(150));
      item.data += "_processed";
      item.processed_stage = 2;
    }, stage2_events.back().get());
  }

  // Wait for stage 2
  for (auto& ev : stage2_events) {
    ev->Wait(2000);
  }
  std::cout << "Stage 2 completed\n\n";

  // Stage 3: Save
  std::vector<std::unique_ptr<Event>> stage3_events;
  for (auto& item : items) {
    stage3_events.push_back(std::make_unique<Event>(false, false));
    ThreadPool::PostTask([&item]() {
      std::cout << "[Stage 3] Saving item " << item.id << ": " << item.data << "\n";
      std::this_thread::sleep_for(milliseconds(50));
      item.processed_stage = 3;
    }, stage3_events.back().get());
  }

  // Wait for stage 3
  for (auto& ev : stage3_events) {
    ev->Wait(2000);
  }
  std::cout << "Stage 3 completed\n\n";

  // Verify all items processed
  std::cout << "Final results:\n";
  for (const auto& item : items) {
    std::cout << "  Item " << item.id << ": " << item.data
              << " (stage " << item.processed_stage << ")\n";
  }

  ThreadPool::DestroyInstance();

  std::cout << "\nTask pipeline example completed\n";
  return 0;
}
// -----------------------------------------------------------------------------
