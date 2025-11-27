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
      exception_handling_(false),
      task_end_event_(nullptr) {
  events_.AddId(ThreadPoolEvent::kTaskStart, false, false);
  events_.AddId(ThreadPoolEvent::kShutdown, false, false);
  events_.AddId(ThreadPoolEvent::kWorkerEnd, false, false);
  worker_thread_ = std::thread(&ThreadWorker::Worker, this);
}
// -----------------------------------------------------------------------------

ThreadWorker::~ThreadWorker() {
  Shutdown();
}
// -----------------------------------------------------------------------------

void ThreadWorker::Start(std::function<void()> task_func,
                         bool exception_handling,
                         Event* task_end_event) {
  std::unique_lock<std::recursive_mutex> auto_unlock(lock_);

  if (!IsRunning()) {
    task_func_0_ = task_func;
    task_func_1_ = nullptr;
    task_param_ = nullptr;
    exception_handling_ = exception_handling;
    task_end_event_ = task_end_event;
    events_.SetId(ThreadPoolEvent::kTaskStart);
  }
}
// -----------------------------------------------------------------------------

void ThreadWorker::Start(std::function<void(void*)> task_func,
                         void* task_param,
                         bool exception_handling,
                         Event* task_end_event) {
  std::unique_lock<std::recursive_mutex> auto_unlock(lock_);

  if (!IsRunning()) {
    task_func_0_ = nullptr;
    task_func_1_ = task_func;
    task_param_ = task_param;
    exception_handling_ = exception_handling;
    task_end_event_ = task_end_event;
    events_.SetId(ThreadPoolEvent::kTaskStart);
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
    events_.SetId(ThreadPoolEvent::kShutdown);

    if (wait_time < 0) {
      worker_thread_.join();
      return;
    }
    else {
      events_.WaitId(ThreadPoolEvent::kWorkerEnd, wait_time);
    }
  }
}
// -----------------------------------------------------------------------------

void ThreadWorker::ClearTask() {
  task_func_0_ = nullptr;
  task_func_1_ = nullptr;
  task_param_ = nullptr;
  exception_handling_ = false;
  task_end_event_ = nullptr;
}
// -----------------------------------------------------------------------------

void ThreadWorker::Worker() {
  std::string event;

  while (true) {
    uint32_t event = events_.WaitAnyId();

    // do task
    if (event == ThreadPoolEvent::kTaskStart) {
      if (IsRunning()) {
        // execute task
        if (exception_handling_) {
          // run with exception handling
          try {
            if (task_func_0_) {
              task_func_0_();
            }
            else if (task_func_1_) {
              task_func_1_(task_param_);
            }

            if (task_end_event_) {
              // notify that task is executed
              if (task_end_event_->HasId(TaskResultEvent::kPass)) {
                task_end_event_->SetId(TaskResultEvent::kPass);
              }
              else {
                task_end_event_->Set();
              }
            }
          }
          catch (...) {
            // notify that task is executed
            if (task_end_event_) {
              if (task_end_event_->HasId(TaskResultEvent::kFail)) {
                task_end_event_->SetId(TaskResultEvent::kFail);
              }
              else {
                task_end_event_->Set();
              }
            }
          }
        }
        else {
          // run without exception handling
          if (task_func_0_) {
            task_func_0_();
          }
          else if (task_func_1_) {
            task_func_1_(task_param_);
          }

          if (task_end_event_) {
            // notify that task is executed
            if (task_end_event_->HasId(TaskResultEvent::kPass)) {
              task_end_event_->SetId(TaskResultEvent::kPass);
            }
            else {
              task_end_event_->Set();
            }
          }
        }

        ClearTask();

        // notify thread pool this worker is free now
        thread_pool_events_.SetId(ThreadPoolEvent::kStageChange);
      }
    }
    else if (event == ThreadPoolEvent::kShutdown) {
      // need shutdown
      break;
    }
  }

  events_.SetId(ThreadPoolEvent::kWorkerEnd);
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
};  // namespace thread_pool
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
};  // namespace process
// -----------------------------------------------------------------------------
