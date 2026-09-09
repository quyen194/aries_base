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
// Non-template, implemented entirely in .cpp -> windows.h/csignal
// never leak into this header.
class ExitProcessHandleBackend {
 public:
  using Callback = std::function<void()>;

  // Registers the callback to run when the process receives an exit signal.
  // Note: only one callback is supported at a time (same behavior as before).
  static void RegisterHandler(Callback cb);

 private:
  DISALLOW_COPY_AND_ASSIGN(ExitProcessHandleBackend);
  ExitProcessHandleBackend() = delete;
};
// -----------------------------------------------------------------------------

template<typename F>
class ExitProcessHandle {
 public:
  explicit ExitProcessHandle(F&& fn_cleanup)
      : fn_cleanup_(std::forward<F>(fn_cleanup)) {
    ExitProcessHandleBackend::RegisterHandler([this]() { fn_cleanup_(); });
  }

 private:
  F fn_cleanup_;

 private:
  DISALLOW_COPY_AND_ASSIGN(ExitProcessHandle);
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
#endif  // ARIES_BASE_PROCESS_UTILS_EXIT_PROCESS_HANDLE_HPP
// -----------------------------------------------------------------------------
