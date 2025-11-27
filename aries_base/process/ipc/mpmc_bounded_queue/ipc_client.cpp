/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/25 15:31
  filename:  aries_base/process/ipc/mpmc_bounded_queue/ipc_client.cpp

  purpose:   Multi-Producer Multi-Consumer IPC client
*********************************************************************/


// -----------------------------------------------------------------------------
#include <chrono>
#include <cstdint>
#include <cstring>
#include <string>
#include <thread>

#if defined(_WIN32)
// Windows-specific headers
#include <windows.h>
#else
// Unix-specific headers
#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>
#endif

#include "aries_base/process/utils/system.hpp"

#include "aries_base/process/ipc/mpmc_bounded_queue/ipc_client.hpp"
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
using namespace aries_base::process::utils;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

IpcClient::IpcClient()
    : server_name_(""),
#if defined(_WIN32)
      windows_handle_(nullptr),
#else
      unix_fd_(-1),
      unix_map_(nullptr),
      unix_map_size_(0),
#endif
      shared_memory_(nullptr) {}
// -----------------------------------------------------------------------------

IpcClient::~IpcClient() {}
// -----------------------------------------------------------------------------

void IpcClient::SetName(const std::string& name_prefix,
                        bool cross_process,
                        uint32_t port_number) {
  if (cross_process) {
    // cross-process: "/<name_prefix>_<port_number>"
    server_name_ = "/_" + name_prefix;
    server_name_ += "_" + std::to_string(port_number);
  }
  else {
    // same-process: "<name_prefix>_<process_id>_<thread_id>"
    server_name_ = "_" + name_prefix;
    uint64_t process_id = static_cast<uint64_t>(GetPID());
    server_name_ += "_" + std::to_string(process_id);
    uint64_t thread_id = static_cast<uint64_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    server_name_ += "_" + std::to_string(thread_id);
  }
}
// -----------------------------------------------------------------------------

void IpcClient::SetNameEx(const std::string& name) {
  server_name_ = name;
}
// -----------------------------------------------------------------------------

bool IpcClient::Connect() {
#if defined(_WIN32)
  // Windows-specific connection logic would go here
#else
  // Unix-specific connection logic
  unix_fd_ = shm_open(server_name_.c_str(), O_RDWR, 0666);
  if (unix_fd_ == -1) {
    // Failed to open shared memory
    return false;
  }

  // temporarily map to get size
  unix_map_size_ = sizeof(SharedMemory);
  // map the shared memory region into the process address space
  void* unix_map = mmap(nullptr, unix_map_size_, PROT_READ | PROT_WRITE, MAP_SHARED, unix_fd_, 0);
  if (unix_map == MAP_FAILED) {
    // mapping failed, cleanup
    close(unix_fd_);
    unix_fd_ = -1;
    return false;
  }

  // store mapping in file-scoped variables
  SharedMemory* shared_memory = reinterpret_cast<SharedMemory*>(unix_map);

  // calculate full size
  unix_map_size_ = shared_memory->MemorySizeNeeded();

  // unmap temporary mapping
  munmap(unix_map, sizeof(SharedMemory));

  // remap with full size
  unix_map_ = mmap(nullptr, unix_map_size_, PROT_READ | PROT_WRITE, MAP_SHARED, unix_fd_, 0);
  if (unix_map_ == MAP_FAILED) {
    // mapping failed, cleanup
    close(unix_fd_);
    unix_fd_ = -1;
    return false;
  }

  // store mapping in file-scoped variables
  shared_memory_ = reinterpret_cast<SharedMemory*>(unix_map_);
#endif

  return true;
}
// -----------------------------------------------------------------------------

void IpcClient::Disconnect() {
#if defined(_WIN32)
  // Windows-specific disconnection logic would go here
#else
  // Unix-specific disconnection logic
  shared_memory_ = nullptr;

  if (unix_map_ && unix_map_size_ > 0) {
    munmap(unix_map_, unix_map_size_);
    unix_map_ = nullptr;
    unix_map_size_ = 0;
  }
  if (unix_fd_ != -1) {
    close(unix_fd_);
    unix_fd_ = -1;
  }
#endif  // end platform check
}
// -----------------------------------------------------------------------------

bool IpcClient::Enqueue(uint16_t msg_id,
                        const void* data,
                        size_t length,
                        uint32_t timeout_ms) {
  if (!shared_memory_ || !shared_memory_->IsWorking()) {
    return false;
  }

  return shared_memory_->Enqueue(msg_id,
                                 reinterpret_cast<const uint8_t*>(data),
                                 length,
                                 timeout_ms);
}
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
