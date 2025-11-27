/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/18 06:02
  filename:  aries_base/process/ipc/mpmc_bounded_queue/ipc_server.cpp

  purpose:   Multi-Producer Single-Consumer IPC server
*********************************************************************/


// -----------------------------------------------------------------------------
#include <atomic>
#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>

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

#include "aries_base/process/thread_pool/thread_pool.hpp"
#include "aries_base/process/ipc/mpmc_bounded_queue/ipc_server.hpp"
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
using namespace std::chrono;
using namespace process::utils;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

IpcServer::IpcServer()
    : server_name_(""),
      ipc_type_(IpcType::kUnknown),
#if defined(_WIN32)
      windows_handle_(nullptr),
#else
      unix_fd_(-1),
      unix_map_(nullptr),
      unix_map_size_(0),
#endif
      shared_memory_(nullptr),
      working_(false) {
  // initialize file-scope state if necessary
}
// -----------------------------------------------------------------------------

IpcServer::~IpcServer() {}
// -----------------------------------------------------------------------------

void IpcServer::SetName(const std::string& name_prefix,
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

void IpcServer::SetIpcType(IpcType type) {
  ipc_type_ = type;
}
// -----------------------------------------------------------------------------

bool IpcServer::Create(uint32_t block_size, uint32_t block_count) {
  if (server_name_.empty()) {
    // server name not set
    return false;
  }

  if (ipc_type_ == IpcType::kUnknown) {
    // ipc type not set
    return false;
  }

#if defined(_WIN32)
  // Windows-specific disconnection logic would go here
#else
  // Unix-specific disconnection logic
  shm_unlink(server_name_.c_str());

  unix_fd_ = shm_open(server_name_.c_str(), O_CREAT | O_RDWR, 0666);
  if (unix_fd_ == -1) {
    // Handle error
    return false;
  }

  // calculate required size
  unix_map_size_ = sizeof(SharedMemory) + block_size * block_count;

  // Set size for the shared memory segment
  ftruncate(unix_fd_, unix_map_size_);

  // map the shared memory region into the process address space
  unix_map_ = mmap(nullptr, unix_map_size_, PROT_READ | PROT_WRITE, MAP_SHARED, unix_fd_, 0);
  if (unix_map_ == MAP_FAILED) {
    // mapping failed, cleanup
    close(unix_fd_);
    unix_fd_ = -1;
    return false;
  }

  // store mapping in file-scoped variables
  shared_memory_ = reinterpret_cast<SharedMemory*>(unix_map_);

  // zero the memory region
  std::memset(unix_map_, 0, unix_map_size_);
#endif  // end platform check

  // setup shared memory structure
  new (shared_memory_) SharedMemory(ipc_type_, block_size, block_count);

  // Notify derived class
  OnCreated();

  return true;
}
// -----------------------------------------------------------------------------

void IpcServer::Destroy() {
  // Signal worker to stop
  shared_memory_->SetWorking(false);

  // Stop worker thread
  working_ = false;

  if (!worker_thread_end_event_) {
    // when worker is run in the same thread, just cleanup directly
    DestroyInternal();
    return;
  }

  // Wait for worker thread to finish
  worker_thread_end_event_->Wait();
}
// -----------------------------------------------------------------------------

void IpcServer::DestroyInternal() {
#if defined(_WIN32)
  // Windows-specific disconnection logic would go here
#else
  // Unmap and cleanup shared memory
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

  OnDestroy();
}
// -----------------------------------------------------------------------------

void IpcServer::CreateWorker() {
  // Set working flag for clients
  shared_memory_->SetWorking(true);
  // Start worker thread
  worker_thread_end_event_ = std::make_unique<Event>(false, false);
  working_ = true;
  ThreadPool::Instance()->PostTask(
      [this]() {
        this->Worker();
      },
      worker_thread_end_event_.get());
}
// -----------------------------------------------------------------------------

void IpcServer::RunWorker() {
  if (worker_thread_end_event_) {
    // Worker already running in another thread
    return;
  }

  // Set working flag for clients
  shared_memory_->SetWorking(true);
  // Run worker in current thread
  working_ = true;
  Worker();
  // Signal worker to stop
  shared_memory_->SetWorking(false);
}
// -----------------------------------------------------------------------------

void IpcServer::OnCreated() {
  std::cout << "IpcServer created: " << server_name_ << "\n";
}
// -----------------------------------------------------------------------------

void IpcServer::OnDestroy() {
  std::cout << "IpcServer destroyed: " << server_name_ << "\n";
}
// -----------------------------------------------------------------------------

void IpcServer::Worker() {
  if (!shared_memory_ || !shared_memory_->IsValid()) {
    return;
  }

  bool exiting = false;
  uint32_t exit_counter = 0;

  uint16_t msg_id = 0;
  uint32_t length = 0;
  std::vector<uint8_t> buffer;
  buffer.resize(shared_memory_->block_size_);
  uint8_t* data_ptr = buffer.data();
  const uint32_t data_size = static_cast<uint32_t>(buffer.size());

  while (true) {
    if (shared_memory_->Dequeue(msg_id, data_ptr, data_size, length, 100)) {
      // message received
      OnMessageReceived(msg_id, data_ptr, length);
      // reset exit flag
      exiting = false;
    }
    else {
      // no message, sleep briefly
      std::this_thread::sleep_for(milliseconds(10));

      // check for exit condition
      // we wait for one more loop to ensure all messages are processed
      if (exiting) {
        if (++exit_counter >= 5) {
          break;
        }
      }
      else if (!working_) {
        exiting = true;
        exit_counter = 0;
      }
    }
  }

  if (worker_thread_end_event_) {
    // when running in a separate thread, do cleanup here
    DestroyInternal();
  }
}
// -----------------------------------------------------------------------------

// Default message handler - can be overridden
void IpcServer::OnMessageReceived(uint16_t id,
                                  const uint8_t* message,
                                  uint32_t length) {
  std::cout << "IpcServer received message id=" << id << " len=" << length << "\n";
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
