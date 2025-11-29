/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/16 15:32
  filename:  aries_base/process/event/event.hpp

  purpose:   Event synchronization primitive with single-state and
             multi-state support. Uses numeric IDs internally with
             bitset-backed storage for performance, while maintaining
             ergonomic string-based API via interning.
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_PROCESS_EVENT_EVENT_HPP
#define ARIES_BASE_PROCESS_EVENT_EVENT_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <limits>
#include <map>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

#include "aries_base/definitions/macro.hpp"
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace process {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
using namespace std::chrono;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

class Event {
 public:
  Event(bool manual_reset = false, bool initial_state = false);
  virtual ~Event();

  // Single state operations
  bool Set();
  bool Reset();
  bool Wait(int64_t wait_time = -1);

  // Multi-state String-based API (interns to numeric IDs)
  bool AddName(const std::string &state_name, bool manual_reset = false, bool initial_state = false);
  bool RemoveName(const std::string &state_name);
  bool HasName(const std::string &state_name);
  // Signal event by name
  bool SetName(const std::string &state_name);
  // Reset event state by name
  bool ResetName(const std::string &state_name);
  bool WaitName(const std::string &state_name, int64_t wait_time = -1);
  const std::string WaitAnyName(int64_t wait_time = -1);
  const std::string WaitNames(const std::vector<std::string> &state_names, int64_t wait_time = -1);

  // Get numeric ID for a state name (returns INVALID_ID if not found)
  uint32_t GetId(std::string_view name) const;

  // Multi-state Numeric-based API (fast path - use for performance-critical code)
  bool AddId(uint32_t state_id, bool manual_reset = false, bool initial_state = false);
  bool RemoveId(uint32_t state_id);
  bool HasId(uint32_t state_id);
  // Signal event by ID
  bool SetId(uint32_t state_id);
  // Reset event state by ID
  bool ResetId(uint32_t state_id);
  bool WaitId(uint32_t state_id, int64_t timeout_ms = -1);
  uint32_t WaitAnyId(int64_t timeout_ms = -1);
  uint32_t WaitIds(const std::vector<uint32_t> &state_ids, int64_t wait_time = -1);

  // Multi-state wait all
  bool WaitAll(int64_t timeout_ms = -1);

 private:
  // String name interning - one-time conversion to numeric ID
  uint32_t InternName(std::string_view state_name);

  // Index management for bitset storage
  uint32_t IndexOf(uint32_t state_id) const;
  void EnsureCapacityForIndex(uint32_t index);

  // Bitset helper functions (unsafe - caller must hold lock)
  bool GetBitUnsafe(uint32_t index) const;
  bool SetBitUnsafe(uint32_t index);
  bool ClearBitUnsafe(uint32_t index);
  bool AnySetUnsafe() const;
  bool AllSetUnsafe() const;
  uint32_t FirstSetUnsafe() const;

 private:
  bool is_single_state_;
  std::mutex lock_;
  std::condition_variable condition_;

  // String name to numeric ID mapping (for interning)
  std::map<std::string, uint32_t> name_to_id_;
  uint32_t next_id_;

  // Numeric ID to bitset index mapping
  std::map<uint32_t, uint32_t> id_to_index_;
  uint32_t next_index_;

  // Bitset-backed state storage (64-bit words) - raw array since atomic is not copyable
  std::atomic<uint64_t>* bits_;
  uint32_t bits_capacity_; // number of 64-bit words allocated

  // Manual reset flags (indexed by bitset index)
  std::vector<bool> manual_resets_;

 public:
  static const uint32_t INVALID_ID;

 private:
  DISALLOW_COPY_AND_ASSIGN(Event);
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace process
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_PROCESS_EVENT_EVENT_HPP
// -----------------------------------------------------------------------------
