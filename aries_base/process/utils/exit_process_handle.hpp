/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/12/29 16:58
  filename:  aries_base/process/utils/exit_process_handle.hpp

  purpose:   Helper class for performing cleanup actions at scope exit
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_PROCESS_UTILS_EXIT_PROCESS_HANDLE_HPP
#define ARIES_BASE_PROCESS_UTILS_EXIT_PROCESS_HANDLE_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#else // UNIX/POSIX
#include <csignal>
#endif  // OS Specific includes

#include <functional>

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

template<typename F>
class ExitProcessHandle {
 public:
  explicit ExitProcessHandle(F&& fn_cleanup)
      : fn_cleanup_(std::forward<F>(fn_cleanup)) {
    // Store instance pointer
    instance_ = this;

#ifdef _WIN32
    // Set console control handler
    SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
#else // UNIX/POSIX
    // Set signal handlers for common termination signals
    signal(SIGINT, SignalHandle);
    signal(SIGTERM, SignalHandle);
    signal(SIGHUP, SignalHandle);
#endif  // OS Specific
  }

 private:
#ifdef _WIN32
  static BOOL WINAPI ConsoleCtrlHandler(DWORD ctrl_type) {
    switch (ctrl_type) {
      case CTRL_C_EVENT:
      case CTRL_CLOSE_EVENT:
      case CTRL_BREAK_EVENT:
      case CTRL_LOGOFF_EVENT:
      case CTRL_SHUTDOWN_EVENT:
        if (instance_) {
          instance_->fn_cleanup_();
        }
        return TRUE;  // Indicate that the signal has been handled
      default:
        return FALSE; // Pass other signals to the next handler
    }
  }
#else // UNIX/POSIX
  static void SignalHandle(int signum) {
    if (instance_) {
      instance_->fn_cleanup_();
    }
  }
#endif // OS Specific

 private:
  F fn_cleanup_;

 private:
  static ExitProcessHandle* instance_;

 private:
  DISALLOW_COPY_AND_ASSIGN(ExitProcessHandle);
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
template<typename F>
ExitProcessHandle<F>* ExitProcessHandle<F>::instance_ = nullptr;
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
#endif  // ARIES_BASE_PROCESS_UTILS_EXIT_PROCESS_HANDLE_HPP
// -----------------------------------------------------------------------------
