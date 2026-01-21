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
#ifdef _WIN32
  #include <windows.h>
#else
  #include <sys/file.h>
  #include <fcntl.h>
  #include <unistd.h>
#endif

#include <filesystem>
#include <iostream>
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
  explicit SingleProcess(const std::string& lock_filename = "process.lock", bool auto_acquire = true)
      : lock_filename_(lock_filename),
#ifdef _WIN32
        handle_(INVALID_HANDLE_VALUE),
#else // UNIX/POSIX
        fd_(-1),
#endif
        locked_(false) {
    if (auto_acquire) {
      AcquireLock();
    }
  }

  ~SingleProcess() {
    ReleaseLock();
  }

  // Attempt to acquire the lock
  // Returns true if lock was successfully acquired
  bool AcquireLock() {
#ifdef _WIN32
    handle_ = CreateMutexA(NULL, FALSE, lock_filename_.c_str());
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
      return false;
    }

#else // UNIX/POSIX
    fd_ = open(lock_filename_.c_str(), O_CREAT | O_RDWR, 0644);
    if (fd_ == -1) {
      return false;
    }

    // Try to acquire exclusive, non-blocking lock
    if (flock(fd_, LOCK_EX | LOCK_NB) == -1) {
      close(fd_);
      fd_ = -1;
      return false;
    }
#endif

    locked_ = true;

    std::cout << "lock accquired with " << lock_filename_ << std::endl;

    return true;
  }

  void ReleaseLock() {
    if (!locked_) {
      return;
    }

#ifdef _WIN32
    if (handle_ != INVALID_HANDLE_VALUE) {
      CloseHandle(handle_);
      handle_ = INVALID_HANDLE_VALUE;
    }
#else // UNIX/POSIX
    if (fd_ != -1) {
      close(fd_);
      fd_ = -1;
    }
    std::filesystem::remove(lock_filename_);
    locked_ = false;

    std::cout << "lock released for " << lock_filename_ << std::endl;
#endif
  }

  // Returns true if another instance is currently holding the lock
  bool AnotherInstanceIsRunning() const {
    return !locked_;
  }

 private:
  std::string lock_filename_;
#ifdef _WIN32
  HANDLE handle_ = INVALID_HANDLE_VALUE;
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
