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

  bool Add(const std::string &state_name, bool manual_reset = false, bool initial_state = false);
  void Remove(const std::string &state_name);
  bool Has(const std::string &state_name);

  // single state
  bool Set();
  bool Reset();
  bool Wait(int64_t wait_time = -1);

  // multi state
  bool Set(const std::string &state_name);
  bool Reset(const std::string &state_name);
  bool Wait(const std::string &state_name, int64_t wait_time = -1);
  const std::string WaitAny(int64_t wait_time = -1);
  bool WaitAll(int64_t wait_time = -1);

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
#endif  // ARIES_BASE_PROCESS_EVENT_EVENT_HPP
// -----------------------------------------------------------------------------