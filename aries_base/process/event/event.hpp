/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/16 15:32
  filename:  aries_base/process/event/event.hpp

  purpose:
*********************************************************************/



// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_PROCESS_EVENT_EVENT_HPP
#define ARIES_BASE_PROCESS_EVENT_EVENT_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <condition_variable>
#include <chrono>
#include <cstdint>
#include <limits>
#include <map>
#include <mutex>
#include <string>
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace process {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

class Event {
 public:
  Event(bool manual_reset = false, bool initial_state = false);
  virtual ~Event();

  bool Add(const std::string &state_name, bool manual_reset = false, bool initial_state = false);
  void Remove(const std::string &state_name);
  bool Has(const std::string &state_name);

  // single state
  bool Set();
  bool Reset();
  bool Wait(uint64_t wait_time = std::numeric_limits<uint64_t>::max());

  // multi state
  bool Set(const std::string &state_name);
  bool Reset(const std::string &state_name);
  bool Wait(const std::string &state_name, uint64_t wait_time = std::numeric_limits<uint64_t>::max());
  const std::string WaitAny(uint64_t wait_time = std::numeric_limits<uint64_t>::max());
  bool WaitAll(uint64_t wait_time = std::numeric_limits<uint64_t>::max());

 private:
  bool is_single_state_;
  std::mutex lock_;
  std::condition_variable condition_;
  std::map<std::string, bool> states_;
  std::map<std::string, bool> manual_resets_;
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