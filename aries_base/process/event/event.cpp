/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/16 16:23
  filename:  aries_base/process/event/event.cpp

  purpose:   Event implementation using numeric IDs and bitset-backed
             storage for performance. Interns string names to uint32_t
             IDs on first use.
*********************************************************************/


// -----------------------------------------------------------------------------
#include <algorithm>
#include <cstring>

#include "aries_base/process/event/event.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace process {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
using namespace std::chrono;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#define SINGLE_EVENT_NAME "single"
#define SINGLE_EVENT_ID 0
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
const uint32_t Event::INVALID_ID = UINT32_MAX;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

Event::Event(bool manual_reset, bool initial_state)
  : is_single_state_(true),
    next_id_(SINGLE_EVENT_ID + 1),
    next_index_(0),
    bits_(nullptr),
    bits_capacity_(0) {
  // Intern single state name and allocate its bit
  name_to_id_[SINGLE_EVENT_NAME] = SINGLE_EVENT_ID;
  id_to_index_[SINGLE_EVENT_ID] = next_index_;
  next_index_++;

  // Allocate first bitset word
  bits_capacity_ = 1;
  bits_ = new std::atomic<uint64_t>[bits_capacity_];
  bits_[0].store(initial_state ? 1ULL : 0ULL, std::memory_order_relaxed);
  manual_resets_.push_back(manual_reset);
}
// -----------------------------------------------------------------------------

Event::~Event() {
  if (bits_) {
    delete[] bits_;
  }
}
// -----------------------------------------------------------------------------

bool Event::Set() {
  if (is_single_state_) {
    return SetId(SINGLE_EVENT_ID);
  }
  else if (id_to_index_.size() == 1) {
    return SetId(id_to_index_.begin()->first);
  }
  return false;
}
// -----------------------------------------------------------------------------

bool Event::Reset() {
  if (is_single_state_) {
    return ResetId(SINGLE_EVENT_ID);
  }
  else if (id_to_index_.size() == 1) {
    return ResetId(id_to_index_.begin()->first);
  }
  return false;
}
// -----------------------------------------------------------------------------

bool Event::Wait(int64_t wait_time) {
  if (is_single_state_) {
    return WaitId(SINGLE_EVENT_ID, wait_time);
  }
  else if (id_to_index_.size() == 1) {
    return WaitId(id_to_index_.begin()->first, wait_time);
  }
  return false;
}
// -----------------------------------------------------------------------------

bool Event::AddName(const std::string &state_name, bool manual_reset, bool initial_state) {
  std::unique_lock<std::mutex> lock(lock_);

  // Check if already exists
  if (name_to_id_.find(state_name) != name_to_id_.end()) {
    return false; // Already exists
  }

  // If adding first state to single-state event, switch to multi-state mode
  if (is_single_state_) {
    is_single_state_ = false;
    // Exclude the implicit single-state bit from multi-state checks
    id_to_index_.erase(SINGLE_EVENT_ID);
  }

  // Intern name and allocate index
  uint32_t id = InternName(state_name);
  uint32_t index = next_index_++;
  id_to_index_[id] = index;

  // Ensure capacity
  EnsureCapacityForIndex(index);

  // Set manual reset flag
  manual_resets_[index] = manual_reset;
  // Set initial state bit if requested
  if (initial_state) {
    SetBitUnsafe(index);
  }

  return true;
}
// -----------------------------------------------------------------------------

bool Event::RemoveName(const std::string &state_name) {
  std::unique_lock<std::mutex> lock(lock_);

  auto name_it = name_to_id_.find(state_name);
  if (name_it == name_to_id_.end()) {
    return false;
  }

  uint32_t id = name_it->second;
  auto index_it = id_to_index_.find(id);
  if (index_it != id_to_index_.end()) {
    uint32_t index = index_it->second;
    ClearBitUnsafe(index);
    id_to_index_.erase(index_it);
  }

  name_to_id_.erase(name_it);

  return true;
}
// -----------------------------------------------------------------------------

bool Event::HasName(const std::string& state_name) {
  std::unique_lock<std::mutex> lock(lock_);
  return name_to_id_.find(state_name) != name_to_id_.end();
}
// -----------------------------------------------------------------------------

bool Event::SetName(const std::string &state_name) {
  std::unique_lock<std::mutex> lock(lock_);

  auto it = name_to_id_.find(state_name);
  if (it == name_to_id_.end()) {
    return false;
  }

  uint32_t id = it->second;
  uint32_t index = IndexOf(id);
  if (index == INVALID_ID) {
    return false;
  }

  SetBitUnsafe(index);
  condition_.notify_all();

  return true;
}
// -----------------------------------------------------------------------------

bool Event::ResetName(const std::string &state_name) {
  std::unique_lock<std::mutex> lock(lock_);

  auto it = name_to_id_.find(state_name);
  if (it == name_to_id_.end()) {
    return false;
  }

  uint32_t id = it->second;
  uint32_t index = IndexOf(id);
  if (index == INVALID_ID) {
    return false;
  }

  ClearBitUnsafe(index);

  return true;
}
// -----------------------------------------------------------------------------

bool Event::WaitName(const std::string &state_name, int64_t wait_time) {
  std::unique_lock<std::mutex> lock(lock_);

  auto it = name_to_id_.find(state_name);
  if (it == name_to_id_.end()) {
    return false;
  }

  uint32_t id = it->second;
  uint32_t index = IndexOf(id);
  if (index == INVALID_ID) {
    return false;
  }

  // Wait for specific bit to be set
  auto predicate = [this, index]() {
    return GetBitUnsafe(index);
  };

  bool result;
  if (wait_time < 0) {
    condition_.wait(lock, predicate);
    result = true;
  }
  else {
    result = condition_.wait_for(lock, milliseconds(wait_time), predicate);
  }

  if (result && !manual_resets_[index]) {
    ClearBitUnsafe(index);
  }

  return result;
}
// -----------------------------------------------------------------------------

const std::string Event::WaitAnyName(int64_t wait_time) {
  uint32_t id = WaitAnyId(wait_time);

  if (id == INVALID_ID) {
    return "";
  }

  // Find name for this ID
  std::unique_lock<std::mutex> lock(lock_);
  for (const auto& [name, name_id] : name_to_id_) {
    if (name_id == id) {
      return name;
    }
  }
  return "";
}
// -----------------------------------------------------------------------------

uint32_t Event::GetId(std::string_view name) const {
  std::string key(name);
  auto it = name_to_id_.find(key);
  return (it != name_to_id_.end()) ? it->second : INVALID_ID;
}
// -----------------------------------------------------------------------------

bool Event::AddId(uint32_t id, bool manual_reset, bool initial_state) {
  std::unique_lock<std::mutex> lock(lock_);

  if (id_to_index_.find(id) != id_to_index_.end()) {
    return false; // Already exists
  }

  // If adding first state to single-state event, switch to multi-state mode
  if (is_single_state_) {
    is_single_state_ = false;
    // Exclude the implicit single-state bit from multi-state checks
    id_to_index_.erase(SINGLE_EVENT_ID);
  }

  uint32_t index = next_index_++;
  id_to_index_[id] = index;

  // Ensure capacity
  EnsureCapacityForIndex(index);

  // Set manual reset flag
  manual_resets_[index] = manual_reset;
  // Set initial state bit if requested
  if (initial_state) {
    SetBitUnsafe(index);
  }

  return true;
}
// -----------------------------------------------------------------------------

bool Event::RemoveId(uint32_t id) {
  std::unique_lock<std::mutex> lock(lock_);

  auto it = id_to_index_.find(id);
  if (it == id_to_index_.end()) {
    return false;
  }

  uint32_t index = it->second;
  ClearBitUnsafe(index);
  id_to_index_.erase(it);
  return true;
}
// -----------------------------------------------------------------------------

bool Event::HasId(uint32_t id) {
  std::unique_lock<std::mutex> lock(lock_);
  return id_to_index_.find(id) != id_to_index_.end();
}
// -----------------------------------------------------------------------------

bool Event::SetId(uint32_t id) {
  std::unique_lock<std::mutex> lock(lock_);

  uint32_t index = IndexOf(id);
  if (index == INVALID_ID) {
    return false;
  }

  SetBitUnsafe(index);
  condition_.notify_all();

  return true;
}
// -----------------------------------------------------------------------------

bool Event::ResetId(uint32_t id) {
  std::unique_lock<std::mutex> lock(lock_);

  uint32_t index = IndexOf(id);
  if (index == INVALID_ID) {
    return false;
  }

  ClearBitUnsafe(index);

  return true;
}
// -----------------------------------------------------------------------------

bool Event::WaitId(uint32_t id, int64_t timeout_ms) {
  std::unique_lock<std::mutex> lock(lock_);

  uint32_t index = IndexOf(id);
  if (index == INVALID_ID) {
    return false;
  }

  auto predicate = [this, index]() {
    return GetBitUnsafe(index);
  };

  bool result;
  if (timeout_ms < 0) {
    condition_.wait(lock, predicate);
    result = true;
  }
  else {
    result = condition_.wait_for(lock, milliseconds(timeout_ms), predicate);
  }

  if (result && !manual_resets_[index]) {
    ClearBitUnsafe(index);
  }

  return result;
}
// -----------------------------------------------------------------------------

uint32_t Event::WaitAnyId(int64_t timeout_ms) {
  std::unique_lock<std::mutex> lock(lock_);

  auto predicate = [this]() {
    return AnySetUnsafe();
  };

  bool result;
  if (timeout_ms < 0) {
    condition_.wait(lock, predicate);
    result = true;
  }
  else {
    result = condition_.wait_for(lock, milliseconds(timeout_ms), predicate);
  }

  if (!result) {
    return INVALID_ID;
  }

  uint32_t found_id = FirstSetUnsafe();
  if (found_id != INVALID_ID) {
    uint32_t index = IndexOf(found_id);
    if (index != INVALID_ID && !manual_resets_[index]) {
      ClearBitUnsafe(index);
    }
  }

  return found_id;
}
// -----------------------------------------------------------------------------

bool Event::WaitAll(int64_t timeout_ms) {
  std::unique_lock<std::mutex> lock(lock_);

  auto predicate = [this]() {
    return AllSetUnsafe();
  };

  bool result;
  if (timeout_ms < 0) {
    condition_.wait(lock, predicate);
    result = true;
  }
  else {
    result = condition_.wait_for(lock, milliseconds(timeout_ms), predicate);
  }

  if (result) {
    // Reset all non-manual-reset states
    for (const auto& [id, index] : id_to_index_) {
      if (!manual_resets_[index]) {
        ClearBitUnsafe(index);
      }
    }
  }

  return result;
}
// -----------------------------------------------------------------------------

uint32_t Event::InternName(std::string_view name) {
  std::string key(name);
  auto it = name_to_id_.find(key);
  if (it != name_to_id_.end()) {
    return it->second;
  }
  uint32_t id = next_id_++;
  name_to_id_[key] = id;
  return id;
}
// -----------------------------------------------------------------------------

uint32_t Event::IndexOf(uint32_t id) const {
  auto it = id_to_index_.find(id);
  return (it != id_to_index_.end()) ? it->second : INVALID_ID;
}
// -----------------------------------------------------------------------------

void Event::EnsureCapacityForIndex(uint32_t index) {
  uint32_t word_index = index / 64;
  uint32_t new_capacity = word_index + 1;

  if (new_capacity > bits_capacity_) {
    // Allocate new array
    auto* new_bits = new std::atomic<uint64_t>[new_capacity];

    // Copy old data (atomic loads/stores)
    for (uint32_t i = 0; i < bits_capacity_; ++i) {
      new_bits[i].store(bits_[i].load(std::memory_order_acquire), std::memory_order_relaxed);
    }

    // Initialize new words to 0
    for (uint32_t i = bits_capacity_; i < new_capacity; ++i) {
      new_bits[i].store(0ULL, std::memory_order_relaxed);
    }

    // Swap and delete old
    delete[] bits_;
    bits_ = new_bits;
    bits_capacity_ = new_capacity;
  }

  if (index >= manual_resets_.size()) {
    manual_resets_.resize(index + 1, false);
  }
}
// -----------------------------------------------------------------------------

bool Event::GetBitUnsafe(uint32_t index) const {
  uint32_t word_index = index / 64;
  uint32_t bit_index = index % 64;
  if (word_index >= bits_capacity_) {
    return false;
  }
  uint64_t word = bits_[word_index].load(std::memory_order_acquire);
  return (word & (1ULL << bit_index)) != 0;
}
// -----------------------------------------------------------------------------

bool Event::SetBitUnsafe(uint32_t index) {
  uint32_t word_index = index / 64;
  uint32_t bit_index = index % 64;
  if (word_index >= bits_capacity_) {
    return false;
  }

  uint64_t mask = 1ULL << bit_index;
  bits_[word_index].fetch_or(mask, std::memory_order_release);

  return true;
}
// -----------------------------------------------------------------------------

bool Event::ClearBitUnsafe(uint32_t index) {
  uint32_t word_index = index / 64;
  uint32_t bit_index = index % 64;
  if (word_index >= bits_capacity_) {
    return false;
  }

  uint64_t mask = ~(1ULL << bit_index);
  bits_[word_index].fetch_and(mask, std::memory_order_release);

  return true;
}
// -----------------------------------------------------------------------------

bool Event::AnySetUnsafe() const {
  for (uint32_t i = 0; i < bits_capacity_; ++i) {
    if (bits_[i].load(std::memory_order_acquire) != 0) {
      return true;
    }
  }
  return false;
}
// -----------------------------------------------------------------------------

bool Event::AllSetUnsafe() const {
  // Check all registered IDs have their bits set
  for (const auto& [id, index] : id_to_index_) {
    if (!GetBitUnsafe(index)) {
      return false;
    }
  }
  return true;
}
// -----------------------------------------------------------------------------

uint32_t Event::FirstSetUnsafe() const {
  for (const auto& [id, index] : id_to_index_) {
    if (GetBitUnsafe(index)) {
      return id;
    }
  }
  return INVALID_ID;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace process
// -----------------------------------------------------------------------------
