/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/29 15:55
  filename:  aries_base/process/utils/single_process.hpp

  purpose:   Helper class for ensuring single process execution
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_PROCESS_UTILS_SINGLE_PROCESS_HPP
#define ARIES_BASE_PROCESS_UTILS_SINGLE_PROCESS_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <string>

#include "aries_base/definitions/macro.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace process {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace utils {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

class SingleProcess {
 public:
  explicit SingleProcess(const std::string& lock_filename = "process.lock",
                         bool auto_acquire = true);
  ~SingleProcess();

  // Attempt to acquire the lock
  // Returns true if lock was successfully acquired
  bool AcquireLock();
  void ReleaseLock();

  // Returns true if another instance is currently holding the lock
  bool AnotherInstanceIsRunning() const;

 private:
  std::string lock_filename_;
#ifdef _WIN32
  void* handle_ = (void*)(-1);  // INVALID_HANDLE_VALUE
#else
  int fd_ = -1;
#endif
  bool locked_;

 private:
  DISALLOW_COPY_AND_ASSIGN(SingleProcess);
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace utils
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace process
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
} // namespace aries_base
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_PROCESS_UTILS_SINGLE_PROCESS_HPP
// -----------------------------------------------------------------------------
