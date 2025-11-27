/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/26 05:15
  filename:  aries_base/examples/process/thread_pool/_parallel_processing.cpp

  purpose:   Demonstrates using ThreadPool for parallel data processing. Shows
             how to divide work across multiple threads, wait for completion,
             and collect results efficiently.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <chrono>
#include <iostream>
#include <numeric>
#include <vector>

#include "aries_base/process/event/event.hpp"
#include "aries_base/process/thread_pool/thread_pool.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
using namespace std::chrono;
using namespace process;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

// Simulate expensive computation
int compute_sum(const std::vector<int>& data, size_t start, size_t end) {
  int sum = 0;
  for (size_t i = start; i < end; ++i) {
    sum += data[i];
    // Simulate work
    std::this_thread::sleep_for(microseconds(10));
  }
  return sum;
}

int main() {
  std::cout << "ThreadPool Example: Parallel Processing\n";

  ThreadPool::CreateInstance(4, 16);

  // Create large dataset
  const size_t data_size = 10000;
  std::vector<int> data(data_size);
  std::iota(data.begin(), data.end(), 1);  // Fill with 1, 2, 3, ...

  // Compute sequentially for comparison
  auto seq_start = steady_clock::now();
  int sequential_sum = compute_sum(data, 0, data_size);
  auto seq_time = duration_cast<milliseconds>(
      steady_clock::now() - seq_start).count();

  std::cout << "Sequential sum: " << sequential_sum << " in " << seq_time << "ms\n";

  // Parallel computation
  const int num_chunks = 8;
  size_t chunk_size = data_size / num_chunks;
  std::vector<int> partial_sums(num_chunks);
  std::vector<std::unique_ptr<Event>> events;

  auto par_start = steady_clock::now();

  for (int i = 0; i < num_chunks; ++i) {
    size_t start = i * chunk_size;
    size_t end = (i == num_chunks - 1) ? data_size : start + chunk_size;

    events.push_back(std::make_unique<Event>(false, false));
    ThreadPool::PostTask([&data, &partial_sums, i, start, end]() {
      partial_sums[i] = compute_sum(data, start, end);
      std::cout << "Chunk " << i << " computed: " << partial_sums[i] << "\n";
    }, events.back().get());
  }

  // Wait for all chunks
  for (auto& ev : events) {
    ev->Wait(10000);
  }

  // Combine results
  int parallel_sum = std::accumulate(partial_sums.begin(), partial_sums.end(), 0);
  auto par_time = duration_cast<milliseconds>(
      steady_clock::now() - par_start).count();

  std::cout << "\nParallel sum: " << parallel_sum << " in " << par_time << "ms\n";
  std::cout << "Speedup: " << (static_cast<double>(seq_time) / par_time) << "x\n";
  std::cout << "Results match: " << (sequential_sum == parallel_sum ? "YES" : "NO") << "\n";

  ThreadPool::DestroyInstance();

  std::cout << "\nParallel processing example completed\n";
  return 0;
}
// -----------------------------------------------------------------------------