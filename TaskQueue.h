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
    std::lock_guard<std::mutex> lock(mutex_);
    while (!tasks_.empty()) {
      const auto& runnable = tasks_.front();
      if (runnable) {
        runnable();
        tasks_.pop();
      }
    }
  }

 private:
  std::queue<Runnable> tasks_;
  std::mutex mutex_;
};
