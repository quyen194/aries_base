/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/24 20:49
  filename:  aries_base/process/ipc/mpmc_bounded_queue/internal/shared_memory.hpp

  purpose:   Shared memory structure for MPSC (Multi-Producer Single-Consumer) IPC

             This header defines the shared memory layout and synchronization
             primitives for lock-free inter-process communication using a
             bounded queue. The implementation uses atomic operations and
             sequence numbers for synchronization between multiple producer
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
#ifndef ARIES_BASE_PROCESS_IPC_MPMC_BOUNDED_QUEUE_INTERNAL_SHARED_MEMORY_HPP
#define ARIES_BASE_PROCESS_IPC_MPMC_BOUNDED_QUEUE_INTERNAL_SHARED_MEMORY_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <atomic>
#include <cstddef>
#include <cstdint>

#include "aries_base/definitions/macro.hpp"
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

namespace _IpcType {
enum T : uint64_t {
  kUnknown,
  kSPSC,        // Single-Producer Single-Consumer
  kSPMC,        // Single-Producer Multi-Consumer
  kMPSC,        // Multi-Producer Single-Consumer
  kMPMC,        // Multi-Producer Multi-Consumer
};
}  // namespace _IpcType
typedef _IpcType::T IpcType;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

struct Block {
  alignas(8) std::atomic<uint64_t> sequence;        // sequence number for this block
                                                    // it is used to determine if block is ready to read/write
                                                    // producer sets to N*2 when writing, consumer sets to N*2+1 when reading
                                                    // where N is the block index in the ring buffer
                                                    // this allows lock-free synchronization between producer and consumer
                                                    // using memory_order_acquire/release semantics
                                                    // see https://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue
                                                    // for details
                                                    // Note: we use uint64_t to avoid ABA problem in long-running systems
                                                    // even if the index wraps around, the sequence number will be different
                                                    // after 2^64 operations
  alignas(8) uint16_t message_id;                   // message ID for protocol handling
  alignas(8) uint32_t length;                       // length of valid data in the block
  alignas(8) uint8_t data[1];
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

class SharedMemory {
 public:
  SharedMemory(IpcType ipc_type, size_t block_size, uint32_t block_count);
  ~SharedMemory();

  static constexpr uint32_t MemorySizeNeeded(size_t block_size, uint32_t block_count);
  uint32_t MemorySizeNeeded();

  bool IsValid() const;
  bool IsWorking() const;
  Block& GetBlock(uint64_t index);

  bool Enqueue(uint16_t msg_id, const uint8_t* data, size_t length, uint32_t timeout_ms = -1);
  bool Dequeue(uint16_t& msg_id, uint8_t* buffer, size_t buffer_size, uint32_t& length, uint32_t timeout_ms = -1);

 private:
  void SetWorking(bool working);

  bool mpEnqueue(uint16_t msg_id, const uint8_t* data, size_t length, uint32_t timeout_ms = -1);
  bool mcDequeue(uint16_t& msg_id, uint8_t* buffer, size_t buffer_size, uint32_t& length, uint32_t timeout_ms = -1);

  bool spEnqueue(uint16_t msg_id, const uint8_t* data, size_t length, uint32_t timeout_ms = -1);
  bool scDequeue(uint16_t& msg_id, uint8_t* buffer, size_t buffer_size, uint32_t& length, uint32_t timeout_ms = -1);

 private:
  alignas(8) uint64_t available_;                   // value = 0x0412199519041992 meaning server is initialized
  alignas(8) bool working_;                         // it tells all clients whether server is working
  alignas(8) IpcType ipc_type_;                     // IpcType (1: SPSC, 2: SPMC, MPSC, MPMC)
  alignas(8) uint32_t data_size_;                   // size of data area in each block
  alignas(8) uint32_t block_size_;                  // size of each block (including header)
  alignas(8) uint32_t block_count_;                 // total number of blocks
  alignas(8) uint64_t idx_mask_;                    // mask for wrapping indices
  alignas(8) std::atomic<uint64_t> read_idx_;       // index for next read
  alignas(8) std::atomic<uint64_t> write_idx_;      // index for next write
  alignas(8) Block blocks_[1];

 private:
  static constexpr uint64_t kMagicValue = 0x0412199519041992;

 private:
  friend class IpcServer;

 private:
  DISALLOW_COPY_AND_ASSIGN(SharedMemory);
};
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

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_PROCESS_IPC_MPMC_BOUNDED_QUEUE_INTERNAL_SHARED_MEMORY_HPP
// -----------------------------------------------------------------------------
