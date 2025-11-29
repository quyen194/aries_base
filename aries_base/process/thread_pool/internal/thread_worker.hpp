/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/16 14:34
  filename:  aries_base/process/thread_pool/internal/thread_worker.hpp

  purpose:
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_PROCESS_THREAD_POOL_INTERNAL_THREAD_WORKER_HPP
#define ARIES_BASE_PROCESS_THREAD_POOL_INTERNAL_THREAD_WORKER_HPP
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#include <chrono>
#include <functional>
#include <limits>
#include <mutex>
#include <thread>

#include "aries_base/definitions/macro.hpp"
#include "aries_base/container/linked_list.hpp"
#include "aries_base/process/event/event.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace process {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
class ThreadPool;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace thread_pool {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
using namespace std::chrono;
using namespace container;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

class ThreadWorker : public LinkNode<ThreadWorker> {
 public:
  virtual ~ThreadWorker();

 private:
  ThreadWorker(Event &thread_pool_events);  // for ThreadPool

  void Start(std::function<void()> task_func, Event* task_end_event);
  void Start(std::function<void(void*)> task_func,
             void* task_param,
             Event* task_end_event);
  bool IsRunning();

  bool IsWorking();
  void Shutdown(int64_t wait_time = -1);

 private:
  void ClearTask();
  void Worker();

 private:
  Event &thread_pool_events_;

  std::thread worker_thread_;
  Event events_;

  std::recursive_mutex lock_;
  std::function<void()> task_func_0_;
  std::function<void(void*)> task_func_1_;
  void* task_param_;
  Event *task_end_event_;

 private:
  friend class process::ThreadPool;

 private:
  DISALLOW_COPY_AND_ASSIGN(ThreadWorker);
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
}  // namespace thread_pool
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
}  // namespace process
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
}  // namespace aries_base
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_PROCESS_THREAD_POOL_INTERNAL_THREAD_WORKER_HPP
// -----------------------------------------------------------------------------
