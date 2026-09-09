/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2026/09/07 16:58
  filename:  aries_base/process/utils/exit_process_handle.cpp

  purpose:   Helper class for performing cleanup actions at scope exit
*********************************************************************/


// -----------------------------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#else // UNIX/POSIX
#include <csignal>
#endif  // OS Specific includes

#include <functional>

#include "aries_base/process/utils/exit_process_handle.hpp"
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
namespace {

ExitProcessHandleBackend::Callback& GetCallback() {
  static ExitProcessHandleBackend::Callback cb;
  return cb;
}

#ifdef _WIN32
BOOL WINAPI ConsoleCtrlHandler(DWORD ctrl_type) {
  switch (ctrl_type) {
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
      if (auto& cb = GetCallback()) cb();
      return TRUE;  // Indicate that the signal has been handled
    default:
      return FALSE; // Pass other signals to the next handler
  }
}
#else  // UNIX/POSIX
void SignalHandler(int /*signum*/) {
  if (auto& cb = GetCallback()) cb();
}
#endif  // OS Specific

} // namespace
// -----------------------------------------------------------------------------

void ExitProcessHandleBackend::RegisterHandler(Callback cb) {
  GetCallback() = std::move(cb);

#ifdef _WIN32
  // Set console control handler
  SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
#else  // UNIX/POSIX
  // Set signal handlers for common termination signals
  signal(SIGINT, SignalHandler);
  signal(SIGTERM, SignalHandler);
  signal(SIGHUP, SignalHandler);
#endif  // OS Specific
}
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
