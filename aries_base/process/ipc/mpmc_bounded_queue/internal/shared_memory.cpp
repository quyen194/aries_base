/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/25 05:49
  filename:  aries_base/process/ipc/mpmc_bounded_queue/internal/shared_memory.cpp

  purpose:   Shared memory structure for MPMC (Multi-Producer Single-Consumer) IPC

             This source file implements the shared memory layout and
             synchronization primitives for lock-free inter-process communication
             using a bounded queue. The implementation uses atomic operations
             and sequence numbers for synchronization between multiple producer
             processes and a single consumer process.

             Key features:
             - Lock-free bounded queue using sequence numbers
             - Zero-copy message passing through shared memory
             - ABA problem prevention using 64-bit sequence counters
             - Memory-order semantics for proper synchronization

             The queue uses a ring buffer of blocks, each containing:
             - Atomic sequence number for synchronization
             - Message ID and length for protocol handling
             - Data payload area

             Reference implementation and algorithm details:
             https://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue
*********************************************************************/


// -----------------------------------------------------------------------------
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <thread>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <emmintrin.h>
#endif

#include "aries_base/process/ipc/mpmc_bounded_queue/internal/shared_memory.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace process {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace ipc {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace mpmc_bounded_queue {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
using namespace std::chrono;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

SharedMemory::SharedMemory(IpcType ipc_type, size_t block_size, uint32_t block_count)
    : available_(0),
      working_(false),
      ipc_type_(ipc_type),
      data_size_(static_cast<uint32_t>(block_size - sizeof(Block))),
      block_size_(static_cast<uint32_t>(block_size)),
      block_count_(block_count),
      idx_mask_(block_count - 1),
      read_idx_(0),
      write_idx_(0) {
  // "Size must be power of 2"
  assert((block_count_ & (block_count_ - 1)) == 0);

  // initialize block sequences
  for (uint64_t i = 0; i < block_count; ++i) {
    Block& block = GetBlock(i);
    block.sequence.store(i, std::memory_order_relaxed);
  }

  // mark server as available
  available_ = kMagicValue;
}
// -----------------------------------------------------------------------------

SharedMemory::~SharedMemory() {}
// -----------------------------------------------------------------------------

constexpr uint32_t SharedMemory::MemorySizeNeeded(size_t block_size,
                                                  uint32_t block_count) {
  return sizeof(SharedMemory) - sizeof(SharedMemory::blocks_[0])
      + block_size * block_count;
}
// -----------------------------------------------------------------------------

uint32_t SharedMemory::MemorySizeNeeded() {
  return MemorySizeNeeded(block_size_, block_count_);
}
// -----------------------------------------------------------------------------

bool SharedMemory::IsValid() const {
  return available_ == kMagicValue;
}
// -----------------------------------------------------------------------------

bool SharedMemory::IsWorking() const {
  return IsValid() && working_;
}
// -----------------------------------------------------------------------------

void SharedMemory::SetWorking(bool working) {
  working_ = working;
}
// -----------------------------------------------------------------------------

Block& SharedMemory::GetBlock(uint64_t index) {
  auto base_ptr = reinterpret_cast<uint8_t*>(blocks_);
  return *reinterpret_cast<Block*>(base_ptr + (index & idx_mask_) * block_size_);
}
// -----------------------------------------------------------------------------

bool SharedMemory::Enqueue(uint16_t msg_id,
                           const uint8_t* data,
                           size_t length,
                           uint32_t timeout_ms) {
  switch (ipc_type_) {
    case IpcType::kMPMC:
    case IpcType::kMPSC:
      return mpEnqueue(msg_id, data, length, timeout_ms);
    case IpcType::kSPSC:
    case IpcType::kSPMC:
      return spEnqueue(msg_id, data, length, timeout_ms);
    default:
      return false;
  }
}
// -----------------------------------------------------------------------------

bool SharedMemory::Dequeue(uint16_t& msg_id,
                           uint8_t* buffer,
                           size_t buffer_size,
                           uint32_t& length,
                           uint32_t timeout_ms) {
  switch (ipc_type_) {
    case IpcType::kMPMC:
    case IpcType::kSPMC:
      return mcDequeue(msg_id, buffer, buffer_size, length, timeout_ms);
    case IpcType::kMPSC:
    case IpcType::kSPSC:
      return scDequeue(msg_id, buffer, buffer_size, length, timeout_ms);
    default:
      return false;
  }
}
// -----------------------------------------------------------------------------

bool SharedMemory::mpEnqueue(uint16_t msg_id,
                             const uint8_t* data,
                             size_t length,
                             uint32_t timeout_ms) {
  auto start_time = steady_clock::now();

  do {
    uint64_t write_pos = write_idx_.load(std::memory_order_relaxed);

    Block& block = GetBlock(write_pos);
    uint64_t seq = block.sequence.load(std::memory_order_acquire);
    int64_t diff = static_cast<int64_t>(seq) - static_cast<int64_t>(write_pos);

    if (diff == 0) {
      // block is free for writing
      if (write_idx_.compare_exchange_weak(write_pos,
                                           write_pos + 1,
                                           std::memory_order_release,
                                           std::memory_order_relaxed)) {
        // acquired the block for writing
        block.message_id = msg_id;
        if (length > data_size_) {
          length = data_size_;  // truncate if data is too large
        }
        block.length = static_cast<uint32_t>(length);
        std::memcpy(block.data, data, length);

        // mark block as ready to read
        block.sequence.store(write_pos + 1, std::memory_order_release);

        return true;
      }
    }

    // block is not free, check timeout
    if (timeout_ms > 0) {
      auto now = steady_clock::now();
      auto elapsed_ms = duration_cast<milliseconds>(now - start_time).count();
      if (elapsed_ms >= timeout_ms) {
        return false;  // timeout
      }
    }

    // retry
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
    _mm_pause();
#else
    std::this_thread::yield();
#endif
  } while (true);
}
// -----------------------------------------------------------------------------

bool SharedMemory::mcDequeue(uint16_t& msg_id,
                             uint8_t* buffer,
                             size_t buffer_size,
                             uint32_t& length,
                             uint32_t timeout_ms) {
  auto start_time = steady_clock::now();

  do {
    uint64_t read_pos = read_idx_.load(std::memory_order_relaxed);

    Block& block = GetBlock(read_pos);
    uint64_t seq = block.sequence.load(std::memory_order_acquire);
    int64_t diff = static_cast<int64_t>(seq) - static_cast<int64_t>(read_pos + 1);

    if (diff == 0) {
      // block is ready to read
      if (read_idx_.compare_exchange_weak(read_pos,
                                          read_pos + 1,
                                          std::memory_order_release,
                                          std::memory_order_relaxed)) {
        // acquired the block for reading
        msg_id = block.message_id;
        length = static_cast<uint32_t>(block.length);
        if (length > static_cast<uint32_t>(buffer_size)) {
          length = static_cast<uint32_t>(buffer_size);  // truncate if buffer is too small
        }
        std::memcpy(buffer, block.data, length);

        // mark block as free for writing
        block.sequence.store(read_pos + block_count_, std::memory_order_release);

        return true;
      }
    }

    // no message available, check timeout
    if (timeout_ms > 0) {
      auto now = steady_clock::now();
      auto elapsed_ms = duration_cast<milliseconds>(now - start_time).count();
      if (elapsed_ms >= timeout_ms) {
        return false;  // timeout
      }
    }

    // retry
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
    _mm_pause();
#else
    std::this_thread::yield();
#endif
  } while (true);
}
// -----------------------------------------------------------------------------

bool SharedMemory::spEnqueue(uint16_t msg_id,
                             const uint8_t* data,
                             size_t length,
                             uint32_t timeout_ms) {
  auto start_time = steady_clock::now();

  do {
    uint64_t write_pos = write_idx_;

    Block& block = GetBlock(write_pos);
    uint64_t seq = block.sequence.load(std::memory_order_acquire);
    int64_t diff = static_cast<int64_t>(seq) - static_cast<int64_t>(write_pos);

    if (diff == 0) {
      // block is free for writing
      msg_id = block.message_id;
      if (length > data_size_) {
        length = data_size_;  // truncate if data is too large
      }
      block.length = static_cast<uint32_t>(length);
      std::memcpy(block.data, data, length);

      // mark block as ready to read
      block.sequence.store(write_pos + 1, std::memory_order_release);

      // update write index
      write_idx_ = write_pos + 1;

      return true;
    }

    // block is not free, check timeout
    if (timeout_ms > 0) {
      auto now = steady_clock::now();
      auto elapsed_ms = duration_cast<milliseconds>(now - start_time).count();
      if (elapsed_ms >= timeout_ms) {
        return false;  // timeout
      }
    }

    // retry
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
    _mm_pause();
#else
    std::this_thread::yield();
#endif
  } while (true);
}
// -----------------------------------------------------------------------------

bool SharedMemory::scDequeue(uint16_t& msg_id,
                             uint8_t* buffer,
                             size_t buffer_size,
                             uint32_t& length,
                             uint32_t timeout_ms) {
  auto start_time = steady_clock::now();

  do {
    uint64_t read_pos = read_idx_;

    Block& block = GetBlock(read_pos);
    uint64_t seq = block.sequence.load(std::memory_order_acquire);
    int64_t diff = static_cast<int64_t>(seq) - static_cast<int64_t>(read_pos + 1);

    if (diff == 0) {
      // block is ready to read
      msg_id = block.message_id;
      length = static_cast<uint32_t>(block.length);
      if (length > static_cast<uint32_t>(buffer_size)) {
        length = static_cast<uint32_t>(buffer_size);  // truncate if buffer is too small
      }
      std::memcpy(buffer, block.data, length);

      // mark block as free for writing
      block.sequence.store(read_pos + block_count_, std::memory_order_release);

      // update read index
      read_idx_ = read_pos + 1;

      return true;
    }

    // no message available, check timeout
    if (timeout_ms > 0) {
      auto now = steady_clock::now();
      auto elapsed_ms = duration_cast<milliseconds>(now - start_time).count();
      if (elapsed_ms >= timeout_ms) {
        return false;  // timeout
      }
    }

    // retry
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
    _mm_pause();
#else
    std::this_thread::yield();
#endif
  } while (true);
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
}  // namespace mpmc_bounded_queue
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
}  // namespace ipc
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
}  // namespace process
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
}  // namespace aries_base
// -----------------------------------------------------------------------------
