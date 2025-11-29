/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/16 15:07
  filename:  aries_base/process/thread_pool/internal/thread_worker.cpp

  purpose:
*********************************************************************/


// -----------------------------------------------------------------------------
#include "aries_base/process/thread_pool/internal/definitions.hpp"
#include "aries_base/process/thread_pool/internal/thread_worker.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace aries_base {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace process {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace thread_pool {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
using namespace std::chrono;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

ThreadWorker::ThreadWorker(Event &thread_pool_events)
    : thread_pool_events_(thread_pool_events),
      task_func_0_(nullptr),
      task_func_1_(nullptr),
      task_param_(nullptr),
      task_end_event_(nullptr) {
  events_.Add(TASK_START_EVENT, false, false);
  events_.Add(SHUTDOWN_EVENT, false, false);
  events_.Add(WORKER_END_EVENT, false, false);
  worker_thread_ = std::thread(&ThreadWorker::Worker, this);
}
// -----------------------------------------------------------------------------

ThreadWorker::~ThreadWorker() {
  Shutdown();
}
// -----------------------------------------------------------------------------

void ThreadWorker::Start(std::function<void()> task_func,
                         Event* task_end_event) {
  std::unique_lock<std::recursive_mutex> auto_unlock(lock_);

  if (!IsRunning()) {
    task_func_0_ = task_func;
    task_func_1_ = nullptr;
    task_param_ = nullptr;
    task_end_event_ = task_end_event;
    events_.Set(TASK_START_EVENT);
  }
}
// -----------------------------------------------------------------------------

void ThreadWorker::Start(std::function<void(void*)> task_func,
                         void* task_param,
                         Event* task_end_event) {
  std::unique_lock<std::recursive_mutex> auto_unlock(lock_);

  if (!IsRunning()) {
    task_func_0_ = nullptr;
    task_func_1_ = task_func;
    task_param_ = task_param;
    task_end_event_ = task_end_event;
    events_.Set(TASK_START_EVENT);
  }
}
// -----------------------------------------------------------------------------

bool ThreadWorker::IsRunning() {
  std::unique_lock<std::recursive_mutex> auto_unlock(lock_);

  return (task_func_0_ || task_func_1_);
}
// -----------------------------------------------------------------------------

bool ThreadWorker::IsWorking() {
  return worker_thread_.joinable();
}
// -----------------------------------------------------------------------------

void ThreadWorker::Shutdown(int64_t wait_time) {
  if (IsWorking()) {
    steady_clock::time_point start_time = steady_clock::now();
    events_.Set(SHUTDOWN_EVENT);

    if (wait_time < 0) {
      worker_thread_.join();
      return;
    }
    else {
      events_.Wait(WORKER_END_EVENT, wait_time);
    }
  }
}
// -----------------------------------------------------------------------------

void ThreadWorker::ClearTask() {
  task_func_0_ = nullptr;
  task_func_1_ = nullptr;
  task_param_ = nullptr;
  task_end_event_ = nullptr;
}
// -----------------------------------------------------------------------------

void ThreadWorker::Worker() {
  std::string event;

  while (true) {
    std::string event = events_.WaitAny();

    // do task
    if (event == TASK_START_EVENT) {
      if (IsRunning()) {
        // execute task
        if (task_func_0_) {
          task_func_0_();
        }
        else if (task_func_1_) {
          task_func_1_(task_param_);
        }

        // notify that task is executed
        if (task_end_event_) {
          task_end_event_->Set();
        }

        ClearTask();

        // notify thread pool this worker is free now
        thread_pool_events_.Set(STAGE_CHANGE_EVENT);
      }
    }
    else if (event == SHUTDOWN_EVENT) {
      // need shutdown
      break;
    }
  }

  events_.Set(WORKER_END_EVENT);
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
};  // namespace thread_pool
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
};  // namespace process
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
};  // namespace aries_base
// -----------------------------------------------------------------------------
