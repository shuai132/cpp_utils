#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <utility>

class executor_poll : public coro::executor {
 public:
  executor_poll() = default;
  ~executor_poll() override {
    stop();
  }

  void poll() {
    if (!running_thread_id_.has_value()) {
      running_thread_id_ = std::this_thread::get_id();
    }

    std::function<void()> task;

    {
      std::lock_guard<std::mutex> lock(queue_mutex_);
      if (!stop_.load(std::memory_order_relaxed) && !delayed_task_queue_.empty() &&
          delayed_task_queue_.top().execute_at <= std::chrono::steady_clock::now()) {
        task = std::move(delayed_task_queue_.top().task);
        delayed_task_queue_.pop();
      } else if (!stop_.load(std::memory_order_relaxed) && !task_queue_.empty()) {
        task = std::move(task_queue_.front());
        task_queue_.pop();
      } else {
        return;
      }
    }

    if (task) {
      task();
    }
  }

  template <typename F>
  void post(F&& f) {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    task_queue_.emplace(std::forward<F>(f));
  }

  void dispatch(std::function<void()> fn) override {
    if (std::this_thread::get_id() == running_thread_id_) {
      fn();
    } else {
      post(std::move(fn));
    }
  }

  void post_delayed(std::function<void()> fn, const uint32_t delay) override {
    auto execute_at = std::chrono::steady_clock::now() + std::chrono::milliseconds(delay);
    {
      std::lock_guard<std::mutex> lock(queue_mutex_);
      delayed_task_queue_.push({execute_at, std::move(fn)});
    }
  }

  void stop() final {
    stop_.store(true, std::memory_order_release);
  }

  bool stopped() {
    return stop_.load(std::memory_order_acquire);
  }

 private:
  struct DelayedTask {
    std::chrono::steady_clock::time_point execute_at;
    std::function<void()> task;
  };

  struct DelayedTaskCompare {
    bool operator()(const DelayedTask& lhs, const DelayedTask& rhs) const {
      return lhs.execute_at > rhs.execute_at;
    };
  };

 private:
  std::mutex queue_mutex_;
  std::queue<std::function<void()>> task_queue_;
  std::priority_queue<DelayedTask, std::vector<DelayedTask>, DelayedTaskCompare> delayed_task_queue_;
  std::optional<std::thread::id> running_thread_id_;
  std::atomic<bool> stop_{false};
};
