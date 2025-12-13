#pragma once

#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <utility>

class executor_single_thread : public coro::executor {
 public:
  executor_single_thread() = default;
  ~executor_single_thread() override {
    stop();
  }

 public:
  void run_loop() {
    running_thread_id_ = std::this_thread::get_id();
    for (;;) {
      std::function<void()> task;

      {
        std::unique_lock<std::mutex> lock(queue_mutex_);

        if (stop_ && task_queue_.empty() && delayed_task_queue_.empty()) {
          return;
        }

        if (!delayed_task_queue_.empty() && delayed_task_queue_.top().execute_at <= std::chrono::steady_clock::now()) {
          task = std::move(delayed_task_queue_.top().task);
          delayed_task_queue_.pop();
        } else if (!task_queue_.empty()) {
          task = std::move(task_queue_.front());
          task_queue_.pop();
        } else {
          if (delayed_task_queue_.empty()) {
            condition_.wait(lock, [this] {
              return stop_ || !task_queue_.empty();
            });
          } else {
            auto next_wakeup = delayed_task_queue_.top().execute_at;
            condition_.wait_until(lock, next_wakeup, [this] {
              return stop_ || !task_queue_.empty();
            });
          }

          if (stop_ && task_queue_.empty() && delayed_task_queue_.empty()) {
            return;
          }

          if (!task_queue_.empty()) {
            task = std::move(task_queue_.front());
            task_queue_.pop();
          } else if (!delayed_task_queue_.empty() && delayed_task_queue_.top().execute_at <= std::chrono::steady_clock::now()) {
            task = std::move(delayed_task_queue_.top().task);
            delayed_task_queue_.pop();
          }
        }
      }

      if (task) {
        task();
      }
    }
  }

  template <typename F>
  void post(F&& f) {
    {
      std::lock_guard<std::mutex> lock(queue_mutex_);
      task_queue_.emplace(std::forward<F>(f));
    }
    condition_.notify_one();
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
    condition_.notify_one();
  }

  void stop() final {
    {
      std::lock_guard<std::mutex> lock(queue_mutex_);
      stop_ = true;
    }
    condition_.notify_all();
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
  std::atomic<bool> stop_{false};
  std::condition_variable condition_;
  std::queue<std::function<void()>> task_queue_;

  std::priority_queue<DelayedTask, std::vector<DelayedTask>, DelayedTaskCompare> delayed_task_queue_;
  std::thread::id running_thread_id_{};
};
