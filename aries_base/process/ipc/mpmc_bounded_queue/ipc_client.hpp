/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/25 15:31
  filename:  aries_base/process/ipc/mpmc_bounded_queue/ipc_client.hpp

  purpose:   Multi-Producer Multi-Consumer IPC client
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_PROCESS_IPC_MPMC_BOUNDED_QUEUE_IPC_CLIENT_HPP
#define ARIES_BASE_PROCESS_IPC_MPMC_BOUNDED_QUEUE_IPC_CLIENT_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <string>

#include "aries_base/definitions/macro.hpp"

#include "aries_base/process/ipc/mpmc_bounded_queue/internal/shared_memory.hpp"
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace process {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace ipc {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace mpmc_bounded_queue {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

class IpcClient {
 public:
  IpcClient();
  ~IpcClient();

  void SetName(const std::string& server_name, bool cross_process = false, uint32_t port_number = 0);
  void SetNameEx(const std::string& unique_name);

  bool Connect();
  void Disconnect();

  bool Enqueue(uint16_t msg_id, const void* data = nullptr, size_t length = 0, uint32_t timeout_ms = -1);

 private:
  // user input names
  std::string server_name_;
  // actual unique name used for IPC
  std::string unique_name_;

#if defined(_WIN32)
  // Windows-specific members
  void* wins_fd_;
#else
  // Unix-specific members
  int unix_fd_;
#endif  // end platform check

  void* mem_map_;
  uint32_t map_size_;

  SharedMemory* shared_memory_;

 private:
  DISALLOW_COPY_AND_ASSIGN(IpcClient);
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
}  // namespace mpmc_bounded_queue
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
}  // namespace ipc
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
}  // namespace process
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
}  // namespace aries_base
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_PROCESS_IPC_MPMC_BOUNDED_QUEUE_IPC_CLIENT_HPP
// -----------------------------------------------------------------------------
