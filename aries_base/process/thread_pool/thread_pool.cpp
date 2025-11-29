/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/16 14:34
  filename:  aries_base/process/thread_pool/thread_pool.cpp

  purpose:
*********************************************************************/


// -----------------------------------------------------------------------------
#include <chrono>

#include "aries_base/process/thread_pool/internal/definitions.hpp"
#include "aries_base/process/thread_pool/thread_pool.hpp"
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
namespace process {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
using namespace std::chrono;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
const std::string ThreadPool::TASK_CANCEL_EVENT = "task_cancel_event";
const std::string ThreadPool::TASK_PASS_EVENT = "task_pass_event";
const std::string ThreadPool::TASK_FAIL_EVENT = "task_fail_event";
// -----------------------------------------------------------------------------
ThreadPool* ThreadPool::instance_ = nullptr;
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

ThreadPool::ThreadPool(uint16_t idle_count, uint16_t max_count) {
  idle_thread_count_ = 0;
  max_idle_thread_ = idle_count;
  thread_count_ = 0;
  max_thread_ = max_count;
  events_.Add(STAGE_CHANGE_EVENT, false, false);
  events_.Add(SHUTDOWN_EVENT, false, false);
  worker_thread_ = std::thread(&ThreadPool::Worker, this);

  // allocate
  AllocateWorker();
}
// -----------------------------------------------------------------------------

ThreadPool::~ThreadPool() {
  // stop workers
  while (!free_list_.empty()) {
    ThreadWorker* worker = free_list_.head()->value();
    worker->Free();
    idle_thread_count_--;
    thread_count_--;
  }
  while (!busy_list_.empty()) {
    ThreadWorker* worker = busy_list_.head()->value();
    worker->Free();
    thread_count_--;
  }

  // stop thread pool worker
  events_.Set(SHUTDOWN_EVENT);
  worker_thread_.join();
}
// -----------------------------------------------------------------------------

ThreadPool* ThreadPool::Instance() {
  if (!instance_) {
    CreateInstance(4, 128);
  }
  return instance_;
}
// -----------------------------------------------------------------------------

void ThreadPool::CreateInstance(uint16_t idle_count, uint16_t max_count) {
  if (!instance_) {
    instance_ = new ThreadPool(idle_count, max_count);
  }
}
// -----------------------------------------------------------------------------

void ThreadPool::DestroyInstance() {
  if (instance_) {
    delete instance_;
    instance_ = nullptr;
  }
}
// -----------------------------------------------------------------------------

void ThreadPool::AdjustResource(uint16_t idle_count, uint16_t max_count) {
  instance_->max_idle_thread_ = idle_count;
  instance_->max_thread_ = max_count;
}
// -----------------------------------------------------------------------------

void ThreadPool::PostTask(std::function<void()> task_func,
                          Event* task_end_event) {
  return Instance()->PostTaskImp(task_func, task_end_event);
}
// -----------------------------------------------------------------------------

void ThreadPool::PostTask(std::function<void(void*)> task_func,
                          void* task_param,
                          Event* task_end_event) {
  return Instance()->PostTaskImp(task_func, task_param, task_end_event);
}
// -----------------------------------------------------------------------------

void ThreadPool::PostDelayedTask(std::function<void()> task_func,
                                 int64_t wait_time,
                                 Event* task_end_events) {
  return Instance()->PostDelayedTaskImp(task_func,
                                        wait_time,
                                        task_end_events);
}
// -----------------------------------------------------------------------------

void ThreadPool::PostDelayedTask(std::function<void(void*)> task_func,
                                 void* task_param,
                                 int64_t wait_time,
                                 Event* task_end_events) {
  return Instance()->PostDelayedTaskImp(task_func,
                                        task_param,
                                        wait_time,
                                        task_end_events);
}
// -----------------------------------------------------------------------------

void ThreadPool::PostTaskImp(std::function<void()> task_func,
                             Event* task_end_event) {
  std::unique_lock<std::recursive_mutex> auto_unlock(lock_);

  // no free worker, post as delayed task with 0 wait time
  if (free_list_.empty()) {
    // create pending task
    PendingTask* pending_task = new PendingTask();
    pending_task->task_func_0 = task_func;
    pending_task->task_func_1 = nullptr;
    pending_task->task_param = nullptr;
    pending_task->task_end_event = task_end_event;
    // append to pending list
    pending_tasks_.Append(pending_task);
    return;
  }

  // get free worker
  ThreadWorker* worker = free_list_.head()->value();
  worker->RemoveFromList();
  idle_thread_count_--;
  // move to busy list
  busy_list_.Append(worker);
  // start task
  worker->Start(task_func, task_end_event);

  if (free_list_.empty()) {
    AllocateWorker();
  }
}
// -----------------------------------------------------------------------------

void ThreadPool::PostTaskImp(std::function<void(void*)> task_func,
                             void* task_param,
                             Event* task_end_event) {
  std::unique_lock<std::recursive_mutex> auto_unlock(lock_);

  // no free worker, post as delayed task with 0 wait time
  if (free_list_.empty()) {
    // create pending task
    PendingTask* pending_task = new PendingTask();
    pending_task->task_func_0 = nullptr;
    pending_task->task_func_1 = task_func;
    pending_task->task_param = task_param;
    pending_task->task_end_event = task_end_event;
    // append to pending list
    pending_tasks_.Append(pending_task);
    return;
  }

  // get free worker
  ThreadWorker* worker = free_list_.head()->value();
  worker->RemoveFromList();
  idle_thread_count_--;
  // move to busy list
  busy_list_.Append(worker);
  // start task
  worker->Start(task_func, task_param, task_end_event);

  if (free_list_.empty()) {
    AllocateWorker();
  }
}
// -----------------------------------------------------------------------------

void ThreadPool::PostDelayedTaskImp(std::function<void()> task_func,
                                    int64_t wait_time,
                                    Event* task_end_events) {
  if (task_end_events) {
    task_end_events->Add(TASK_CANCEL_EVENT, false, false);
    task_end_events->Add(TASK_PASS_EVENT, false, false);
    task_end_events->Add(TASK_FAIL_EVENT, false, false);
  }

  std::unique_lock<std::recursive_mutex> auto_unlock(lock_);

  DelayedTask* delayed_task = new DelayedTask();
  delayed_task->task_func_0 = task_func;
  delayed_task->task_func_1 = nullptr;
  delayed_task->task_param = nullptr;
  delayed_task->executing_time = steady_clock::now() + milliseconds(wait_time);
  delayed_task->task_end_events = task_end_events;

  delayed_tasks_.Append(delayed_task);
}
// -----------------------------------------------------------------------------

void ThreadPool::PostDelayedTaskImp(std::function<void(void*)> task_func,
                                    void* task_param,
                                    int64_t wait_time,
                                    Event* task_end_events) {
  if (task_end_events) {
    task_end_events->Add(TASK_CANCEL_EVENT, false, false);
    task_end_events->Add(TASK_PASS_EVENT, false, false);
    task_end_events->Add(TASK_FAIL_EVENT, false, false);
  }

  std::unique_lock<std::recursive_mutex> auto_unlock(lock_);

  DelayedTask* delayed_task = new DelayedTask();
  delayed_task->task_func_0 = nullptr;
  delayed_task->task_func_1 = task_func;
  delayed_task->task_param = task_param;
  delayed_task->executing_time = steady_clock::now() + milliseconds(wait_time);
  delayed_task->task_end_events = task_end_events;

  delayed_tasks_.Append(delayed_task);
}
// -----------------------------------------------------------------------------

void ThreadPool::AllocateWorker() {
  std::unique_lock<std::recursive_mutex> auto_unlock(lock_);

  uint16_t count = max_idle_thread_;
  if (thread_count_ + max_idle_thread_ > max_thread_) {
    count = max_thread_ - thread_count_;
  }

  for (uint16_t i = 0; i < count; i++) {
    free_list_.Append(new ThreadWorker(events_));
    thread_count_++;
    idle_thread_count_++;
  }
}
// -----------------------------------------------------------------------------

void ThreadPool::DeAllocateWorker() {
  std::unique_lock<std::recursive_mutex> auto_unlock(lock_);

  if (idle_thread_count_ >= max_idle_thread_ * 2) {
    while (idle_thread_count_ <= max_idle_thread_) {
      ThreadWorker* worker = free_list_.head()->value();
      worker->Free();
      idle_thread_count_--;
    }
  }
}
// -----------------------------------------------------------------------------

void ThreadPool::Worker() {
  std::string event;

  while (true) {
    std::string event = events_.WaitAny(100);

    // worker thread changed state: busy -> free
    if (event == STAGE_CHANGE_EVENT) {
      std::unique_lock<std::recursive_mutex> auto_unlock(lock_);

      bool check_for_free = false;

      for (LinkNode<ThreadWorker>* node = busy_list_.head();
           node != busy_list_.end_list();
           node = node->next()) {
        ThreadWorker* worker = node->value();
        if (!worker->IsRunning()) {
          node = node->previous();
          worker->RemoveFromList();
          free_list_.Append(worker);
          idle_thread_count_++;
          // if idle thread exceed max, mark for free check
          check_for_free = true;
        }
      }

      // if idle thread exceed max, free some
      if (check_for_free) {
        DeAllocateWorker();
      }
    }

    // check for pending tasks
    {
      std::unique_lock<std::recursive_mutex> auto_unlock(lock_);

      LinkNode<PendingTask>* node = pending_tasks_.head();
      while (node != pending_tasks_.end_list() && !free_list_.empty()) {
        PendingTask* task = node->value();

        // get free worker
        ThreadWorker* worker = free_list_.head()->value();
        worker->RemoveFromList();
        idle_thread_count_--;
        // move to busy list
        busy_list_.Append(worker);
        // start task
        if (task->task_func_0) {
          worker->Start(task->task_func_0, task->task_end_event);
        }
        else if (task->task_func_1) {
          worker->Start(task->task_func_1,
                        task->task_param,
                        task->task_end_event);
        }

        // remove node
        LinkNode<PendingTask>* delete_node = node;
        node = node->previous();
        delete_node->Free();
      }
    }

    // check for delayed tasks
    {
      std::unique_lock<std::recursive_mutex> auto_unlock(lock_);

      steady_clock::time_point now = steady_clock::now();
      LinkNode<DelayedTask>* node = delayed_tasks_.head();
      while (node != delayed_tasks_.end_list()) {
        DelayedTask* task = node->value();
        if ((task->executing_time < now) ||
            ((task->executing_time - now) > hours(24))) {
          // if task is cancelled
          if (task->task_end_events != nullptr &&
              task->task_end_events->Wait(TASK_CANCEL_EVENT, 1)) {
            task->task_end_events->Set(TASK_FAIL_EVENT);
            // remove node
            LinkNode<DelayedTask>* delete_node = node;
            node = node->previous();
            delete_node->Free();
          }
          else {
            // post task
            task->task_end_events->Remove(TASK_CANCEL_EVENT);
            task->task_end_events->Remove(TASK_FAIL_EVENT);

            if (task->task_func_0) {
              PostTaskImp(task->task_func_0, task->task_end_events);
            }
            else if (task->task_func_1) {
              PostTaskImp(task->task_func_1,
                          task->task_param,
                          task->task_end_events);
            }

            // remove node
            LinkNode<DelayedTask>* delete_node = node;
            node = node->previous();
            delete_node->Free();
          }
        }
        node = node->next();
      }
    }

    // need shutdown
    if (event == SHUTDOWN_EVENT) {
      return;
    }
  }
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
}  // namespace process
// -----------------------------------------------------------------------------
