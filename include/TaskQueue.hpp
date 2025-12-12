#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <queue>

class TaskQueue {
  using Runnable = std::function<void()>;

 public:
  TaskQueue() = default;
  TaskQueue(const TaskQueue&) = delete;
  const TaskQueue& operator=(const TaskQueue&) = delete;

  void post(Runnable runnable) {
    std::lock_guard<std::mutex> lock(mutex_);
    tasks_.push(std::move(runnable));
  }

  void poll() {
    while (true) {
      Runnable runnable;
      {
        std::lock_guard<std::mutex> lock(mutex_);
        if (tasks_.empty()) {
          break;
        }
        runnable = std::move(tasks_.front());
        tasks_.pop();
      }
      if (runnable) {
        runnable();
      }
    }
  }

 private:
  std::queue<Runnable> tasks_;
  std::mutex mutex_;
};
