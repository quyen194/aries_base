/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/24 20:48
  filename:  aries_base/process/ipc/mpmc_bounded_queue/ipc_server.hpp

  purpose:   Multi-Producer Single-Consumer IPC server
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_PROCESS_IPC_MPMC_BOUNDED_QUEUE_IPC_SERVER_HPP
#define ARIES_BASE_PROCESS_IPC_MPMC_BOUNDED_QUEUE_IPC_SERVER_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include <cstdint>
#include <memory>
#include <string>

#include "aries_base/definitions/macro.hpp"

#include "aries_base/process/event/event.hpp"

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

class IpcServer {
 public:
  IpcServer();
  ~IpcServer();

  void SetName(const std::string& server_name, bool cross_process = false, uint32_t port_number = 0);
  const std::string& GetName() const { return unique_name_; }

  bool IsCrossProcess() const {
    return !unique_name_.empty() && unique_name_.front() == '/';
  }

  void SetIpcType(IpcType type);

  bool Create(uint32_t block_size = 1024, uint32_t block_count = 1024);
  void Destroy();

  void CreateWorker();
  void RunWorker();

 private:
  virtual void OnCreated();
  virtual void OnDestroy();
  virtual void OnMessageReceived(uint16_t id, const uint8_t* message, uint32_t length);

  void DestroyInternal();
  void Worker();

 private:
  // user input names
  std::string server_name_;
  // actual unique name used for IPC
  std::string unique_name_;
  IpcType ipc_type_;

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

  bool working_;
  std::unique_ptr<Event> worker_thread_end_event_;

 private:
  DISALLOW_COPY_AND_ASSIGN(IpcServer);
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
#endif  // ARIES_BASE_PROCESS_IPC_MPMC_BOUNDED_QUEUE_IPC_SERVER_HPP
// -----------------------------------------------------------------------------
