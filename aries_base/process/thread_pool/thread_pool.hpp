/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/16 11:51
  filename:  aries_base/process/thread_pool/thread_pool.hpp

  purpose:
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_PROCESS_THREAD_POOL_THREAD_POOL_HPP
#define ARIES_BASE_PROCESS_THREAD_POOL_THREAD_POOL_HPP
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#include <cstdint>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

#include "aries_base/definitions/macro.hpp"

#include "aries_base/container/linked_list.hpp"
#include "aries_base/process/event/event.hpp"
#include "aries_base/process/thread_pool/internal/delay_task.hpp"
#include "aries_base/process/thread_pool/internal/pending_task.hpp"
#include "aries_base/process/thread_pool/internal/thread_worker.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace process {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
using namespace thread_pool;
using namespace std::placeholders;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

class ThreadPool {
 public:
  ThreadPool(uint16_t idle_count = 4, uint16_t max_count = 128);
  virtual ~ThreadPool();

  static ThreadPool* Instance();
  static void CreateInstance(uint16_t idle_count, uint16_t max_count);
  static void DestroyInstance();
  static void AdjustResource(uint16_t idle_count, uint16_t max_count);

  static void PostTask(std::function<void()> task_func, Event* task_end_event = nullptr);
  static void PostTask(std::function<void(void*)> task_func,
                       void* task_param,
                       Event* task_end_event = nullptr);
  static void PostDelayedTask(std::function<void()> task_func,
                              int64_t wait_time = -1,
                              Event* task_end_events = nullptr);
  static void PostDelayedTask(std::function<void(void*)> task_func,
                              void* task_param,
                              int64_t wait_time = -1,
                              Event* task_end_events = nullptr);

 public:
  static const std::string TASK_CANCEL_EVENT;  // cancel task from out side
  static const std::string TASK_PASS_EVENT;    // task end after excuted
  static const std::string TASK_FAIL_EVENT;    // task end before excuted (be cancelled)

 private:
  void PostTaskImp(std::function<void()> task_func, Event* task_end_events = nullptr);
  void PostTaskImp(std::function<void(void*)> task_func, void* task_param,
                   Event* task_end_events = nullptr);
  void PostDelayedTaskImp(std::function<void()> task_func,
                          int64_t wait_time = -1,
                          Event* task_end_events = nullptr);
  void PostDelayedTaskImp(std::function<void(void*)> task_func,
                          void* task_param,
                          int64_t wait_time = -1,
                          Event* task_end_events = nullptr);
  void AllocateWorker();
  void DeAllocateWorker();
  void Worker();

 private:
  uint16_t idle_thread_count_;
  uint16_t max_idle_thread_;
  uint16_t thread_count_;
  uint16_t max_thread_;
  LinkedList<ThreadWorker> free_list_;
  LinkedList<ThreadWorker> busy_list_;
  LinkedList<DelayedTask> delayed_tasks_;
  LinkedList<PendingTask> pending_tasks_;
  Event events_;
  std::recursive_mutex lock_;
  std::thread worker_thread_;

  static ThreadPool* instance_;

 private:
  DISALLOW_COPY_AND_ASSIGN(ThreadPool);
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
}  // namespace process
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
}  // namespace aries_base
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_PROCESS_THREAD_POOL_THREAD_POOL_HPP
// -----------------------------------------------------------------------------
