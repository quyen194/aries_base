/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/16 16:23
  filename:  aries_base/process/event/event.cpp

  purpose:
*********************************************************************/


// -----------------------------------------------------------------------------
#include "aries_base/process/event/event.hpp"
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
#define SINGLE_EVENT_NAME "single"
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

Event::Event(bool manual_reset, bool initial_state) {
  // initialize single state
  states_[SINGLE_EVENT_NAME] = initial_state;
  manual_resets_[SINGLE_EVENT_NAME] = manual_reset;
  is_single_state_ = true;
}
// -----------------------------------------------------------------------------

Event::~Event() {}
// -----------------------------------------------------------------------------

bool Event::Add(const std::string &state_name, bool manual_reset, bool initial_state) {
  std::unique_lock<std::mutex> auto_unlock(lock_);

  if (states_.find(state_name) != states_.end()) {
    return false;
  }

  // if adding first state, switch to multi-state mode
  if (is_single_state_) {
    is_single_state_ = false;
    states_.clear();
    manual_resets_.clear();
  }

  // add state
  states_[state_name] = initial_state;
  manual_resets_[state_name] = manual_reset;

  return true;
}
// -----------------------------------------------------------------------------

void Event::Remove(const std::string &state_name) {
  std::unique_lock<std::mutex> auto_unlock(lock_);
  states_.erase(state_name);
  manual_resets_.erase(state_name);
}
// -----------------------------------------------------------------------------

bool Event::Has(const std::string& state_name) {
  std::unique_lock<std::mutex> auto_unlock(lock_);
  return states_.find(state_name) != states_.end();
}
// -----------------------------------------------------------------------------

bool Event::Set() {
  if (is_single_state_) {
    return Set(SINGLE_EVENT_NAME);
  }
  else {
    std::unique_lock<std::mutex> auto_unlock(lock_);
    if (states_.size() == 1) {
      std::string event_name = states_.begin()->first;
      return Set(event_name);
    }
    return false;
  }
}
// -----------------------------------------------------------------------------

bool Event::Reset() {
  if (is_single_state_) {
    return Reset(SINGLE_EVENT_NAME);
  }
  else {
    std::unique_lock<std::mutex> auto_unlock(lock_);
    if (states_.size() == 1) {
      std::string event_name = states_.begin()->first;
      return Reset(event_name);
    }
    return false;
  }
}
// -----------------------------------------------------------------------------

bool Event::Wait(uint64_t wait_time) {
  if (is_single_state_) {
    return Wait(SINGLE_EVENT_NAME, wait_time);
  }
  else {
    std::unique_lock<std::mutex> auto_unlock(lock_);
    if (states_.size() == 1) {
      std::string event_name = states_.begin()->first;
      return Wait(event_name, wait_time);
    }
    return false;
  }
}
// -----------------------------------------------------------------------------

bool Event::Set(const std::string &state_name) {
  std::unique_lock<std::mutex> auto_unlock(lock_);

  if (states_.find(state_name) != states_.end()) {
    states_[state_name] = true;
    condition_.notify_all();
    return true;
  }

  return false;
}
// -----------------------------------------------------------------------------

bool Event::Reset(const std::string &state_name) {
  std::unique_lock<std::mutex> auto_unlock(lock_);

  if (states_.find(state_name) != states_.end()) {
    states_[state_name] = false;
    return true;
  }

  return false;
}
// -----------------------------------------------------------------------------

bool Event::Wait(const std::string &state_name, uint64_t wait_time) {
  std::unique_lock<std::mutex> auto_unlock(lock_);

  // check state exists
  if (states_.find(state_name) == states_.end()) {
    return false;
  }

  // wait for state to be set
  auto predicate = [this, &state_name]() {
    return states_[state_name];
  };

  // wait with timeout
  if (!condition_.wait_for(auto_unlock, milliseconds(wait_time), predicate)) {
    return false;
  }

  // reset state if not manual reset
  if (!manual_resets_[state_name]) {
    states_[state_name] = false;
  }

  return true;
}
// -----------------------------------------------------------------------------

const std::string Event::WaitAny(uint64_t wait_time) {
  std::unique_lock<std::mutex> auto_unlock(lock_);

  // wait for any state to be set
  auto predicate = [this]() {
    for (const auto& state_pair : states_) {
      if (state_pair.second) {
        return true;
      }
    }
    return false;
  };

  // wait with timeout
  if (!condition_.wait_for(auto_unlock, milliseconds(wait_time), predicate)) {
    return "";
  }

  // find and return the first set state
  for (auto& state_pair : states_) {
    if (state_pair.second) {
      // reset state if not manual reset
      if (!manual_resets_[state_pair.first]) {
        state_pair.second = false;
      }
      return state_pair.first;
    }
  }

  return "";
}
// -----------------------------------------------------------------------------

bool Event::WaitAll(uint64_t wait_time) {
  std::unique_lock<std::mutex> auto_unlock(lock_);

  // wait for all states to be set
  auto predicate = [this]() {
    for (const auto& state_pair : states_) {
      if (!state_pair.second) {
        return false;
      }
    }
    return true;
  };

  // wait with timeout
  if (!condition_.wait_for(auto_unlock, milliseconds(wait_time), predicate)) {
    return false;
  }

  // reset states if not manual reset
  for (auto& state_pair : states_) {
    if (!manual_resets_[state_pair.first]) {
      state_pair.second = false;
    }
  }

  return true;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace process
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------
