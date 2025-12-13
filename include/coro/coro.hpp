#pragma once

#include <condition_variable>
#include <coroutine>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <type_traits>
#include <utility>

/// options
// #define CORO_DISABLE_EXCEPTION
#ifndef CORO_DISABLE_EXCEPTION
#include <variant>
#else
#include <optional>
#endif

#ifndef CORO_DEBUG_LEAK_LOG
#define CORO_DEBUG_LEAK_LOG(...) (void)(0)
#endif

#ifndef CORO_DEBUG_LIFECYCLE
#define CORO_DEBUG_LIFECYCLE(...) (void)(0)
#endif

/// compiler check and debug config
#if defined(CORO_DEBUG_PROMISE_LEAK)
#include <cstdio>
#include <unordered_set>
struct debug_coro_promise {
  inline static std::unordered_set<void*> debug_coro_leak;
  static void dump() {
    CORO_DEBUG_LEAK_LOG("debug: debug_coro_leak.size: %zu", debug_coro_leak.size());
  }

  void* operator new(std::size_t size) {
    void* ptr = std::malloc(size);
    CORO_DEBUG_LEAK_LOG("new: %p, size: %zu", ptr, size);
    debug_coro_leak.insert(ptr);
    return ptr;
  }

  void operator delete(void* ptr, [[maybe_unused]] std::size_t size) {
    CORO_DEBUG_LEAK_LOG("free: %p, size: %zu", ptr, size);
    debug_coro_leak.erase(ptr);
    std::free(ptr);
  }
};
#else
struct debug_coro_promise {};
#endif

/// coroutine types
namespace coro {
template <typename T>
struct awaitable;

template <typename T>
struct awaitable_promise;

template <typename T>
struct callback_awaiter;
}  // namespace coro

/// executor
namespace coro {
struct executor {
  std::mutex queue_mutex_;
  std::atomic<bool> stop_{false};
  std::condition_variable condition_;
  std::queue<std::function<void()>> task_queue_;
  std::thread::id running_thread_id_{};

  void run_loop() {
    running_thread_id_ = std::this_thread::get_id();
    for (;;) {
      std::function<void()> task;
      {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        condition_.wait(lock, [this] {
          return stop_ || !task_queue_.empty();
        });

        if (stop_ && task_queue_.empty()) {
          return;
        }

        task = std::move(task_queue_.front());
        task_queue_.pop();
      }

      task();
    }
  }

  void stop() {
    {
      std::lock_guard<std::mutex> lock(queue_mutex_);
      stop_ = true;
    }
    condition_.notify_all();
  }

  ~executor() {
    stop();
  }

  template <typename F>
  void post(F&& f) {
    {
      std::lock_guard<std::mutex> lock(queue_mutex_);
      task_queue_.emplace(std::forward<F>(f));
    }
    condition_.notify_one();
  }

  template <typename F>
  void dispatch(F&& f) {
    if (std::this_thread::get_id() == running_thread_id_) {
      f();
    } else {
      post(std::forward<F>(f));
    }
  }
};
}  // namespace coro

namespace coro {

template <typename T>
struct awaitable_promise_value {
  void return_value(T&& val) noexcept {
    value_.template emplace<T>(std::forward<T>(val));
  }

  void unhandled_exception() noexcept {
#ifndef CORO_DISABLE_EXCEPTION
    value_.template emplace<std::exception_ptr>(std::current_exception());
#endif
  }

  T get_value() const {
#ifndef CORO_DISABLE_EXCEPTION
    if (std::holds_alternative<std::exception_ptr>(value_)) {
      std::rethrow_exception(std::get<std::exception_ptr>(value_));
    }
    return std::get<T>(value_);
#else
    return value_.value();
#endif
  }

#ifndef CORO_DISABLE_EXCEPTION
  std::variant<std::exception_ptr, T> value_{nullptr};
#else
  std::optional<T> value_;
#endif
};

template <>
struct awaitable_promise_value<void> {
  void return_void() noexcept {}

  void unhandled_exception() noexcept {
#ifndef CORO_DISABLE_EXCEPTION
    exception_ = std::current_exception();
#endif
  }

  void get_value() const {
#ifndef CORO_DISABLE_EXCEPTION
    if (exception_) {
      std::rethrow_exception(exception_);
    }
#endif
  }

#ifndef CORO_DISABLE_EXCEPTION
  std::exception_ptr exception_{nullptr};
#endif
};

template <typename T>
struct final_awaitable {
  awaitable_promise<T>* self;

  bool await_ready() noexcept {
    CORO_DEBUG_LIFECYCLE("final_awaitable: await_ready: p: %p, p.h: %p", self, self->parent_handle_.address());
    return false;
  }

  std::coroutine_handle<> await_suspend(std::coroutine_handle<awaitable_promise<T>> h) noexcept {
    CORO_DEBUG_LIFECYCLE("final_awaitable: await_suspend: p: %p, h: %p, p.h: %p", self, h.address(), h.promise().parent_handle_.address());
    CORO_DEBUG_LIFECYCLE("done: %d", h.done());
    // h.promise() is self
    if (h.promise().parent_handle_) {
      return h.promise().parent_handle_;
    } else {
      if (h.done() && !h.promise().awaitable_) {
        h.destroy();
      }
      return std::noop_coroutine();
    }
  }

  void await_resume() noexcept {
    CORO_DEBUG_LIFECYCLE("final_awaitable: await_resume: %p, p.h: %p", self, self->parent_handle_.address());
    self->get_value();
  }
};

template <typename T>
struct awaitable_promise : awaitable_promise_value<T>, debug_coro_promise {
  awaitable<T> get_return_object();

  std::suspend_always initial_suspend() {
    CORO_DEBUG_LIFECYCLE("promise: initial_suspend: p: %p, p.h: %p", this, parent_handle_.address());
    return {};
  }

  final_awaitable<T> final_suspend() noexcept {
    CORO_DEBUG_LIFECYCLE("promise: final_suspend: p: %p, p.h: %p", this, parent_handle_.address());
    return final_awaitable<T>{this};
  }

  std::coroutine_handle<> parent_handle_{};
  executor* executor_ = nullptr;
  awaitable<T>* awaitable_ = nullptr;
};

template <typename T>
struct awaitable {
  using promise_type = awaitable_promise<T>;

  explicit awaitable(std::coroutine_handle<promise_type> h) : current_coro_handle_(h) {
    CORO_DEBUG_LIFECYCLE("awaitable: new: %p, h: %p", this, h.address());
    h.promise().awaitable_ = this;
  }
  ~awaitable() {
    CORO_DEBUG_LIFECYCLE("awaitable: free: %p, h: %p, done: %s", this, current_coro_handle_ ? current_coro_handle_.address() : nullptr,
                         current_coro_handle_ ? current_coro_handle_.done() ? "yes" : "no" : "null");
    if (current_coro_handle_) {
      if (current_coro_handle_.done()) {
        current_coro_handle_.destroy();
      } else {
        current_coro_handle_.promise().awaitable_ = nullptr;
      }
    }
  }

  /// disable copy
  awaitable(const awaitable&) = delete;
  awaitable(awaitable&) = delete;
  awaitable& operator=(const awaitable&) = delete;
  awaitable& operator=(awaitable&) = delete;

  /// enable move
  awaitable(awaitable&& other) noexcept : current_coro_handle_(other.current_coro_handle_) {
    CORO_DEBUG_LIFECYCLE("awaitable: move(c): %p to %p, h: %p", &other, this, current_coro_handle_.address());
    other.current_coro_handle_ = nullptr;
  }
  awaitable& operator=(awaitable&& other) noexcept {
    CORO_DEBUG_LIFECYCLE("awaitable: move(=): %p to %p, h: %p", &other, this, current_coro_handle_.address());
    if (this != &other) {
      if (current_coro_handle_) current_coro_handle_.destroy();
      current_coro_handle_ = other.current_coro_handle_;
      other.current_coro_handle_ = nullptr;
    }
    return *this;
  }

  /// co_await
  bool await_ready() const noexcept {
    CORO_DEBUG_LIFECYCLE("awaitable: await_ready: %p, h: %p", this, current_coro_handle_.address());
    return false;
  }

  template <typename Promise>
  auto await_suspend(std::coroutine_handle<Promise> h) {
    CORO_DEBUG_LIFECYCLE("awaitable: await_suspend: %p, h: %p, p.h: %p", this, current_coro_handle_.address(), h.address());
    current_coro_handle_.promise().executor_ = h.promise().executor_;
    current_coro_handle_.promise().parent_handle_ = h;
    return current_coro_handle_;
  }

  T await_resume() {
    CORO_DEBUG_LIFECYCLE("awaitable: await_resume: %p, h: %p", this, current_coro_handle_.address());
    return current_coro_handle_.promise().get_value();
  }

  auto detach(auto& executor) {
    current_coro_handle_.promise().executor_ = &executor;
    CORO_DEBUG_LIFECYCLE("awaitable: resume begin: %p, h: %p", this, current_coro_handle_.address());
    current_coro_handle_.resume();
    CORO_DEBUG_LIFECYCLE("awaitable: resume end: %p, h: %p", this, current_coro_handle_.address());
  }

  template <typename Function>
  auto detach_with_callback(auto& executor, Function completion_handler) {
    // todo: maybe exception
    auto launched_coro = [](awaitable<T> lazy, auto completion_handler) mutable -> awaitable<void> {
      if constexpr (std::is_void_v<T>) {
        co_await std::move(lazy);
        completion_handler();
      } else {
        completion_handler(co_await std::move(lazy));
      }
    }(std::move(*this), std::move(completion_handler));
    return launched_coro.detach(executor);
  }

  std::coroutine_handle<promise_type> current_coro_handle_;
};

template <typename T>
awaitable<T> awaitable_promise<T>::get_return_object() {
  auto handle = std::coroutine_handle<awaitable_promise<T>>::from_promise(*this);
  CORO_DEBUG_LIFECYCLE("promise: get_return_object: p: %p, h: %p", this, handle.address());
  return awaitable<T>{handle};
}

template <typename T>
struct callback_awaiter_base {
  using callback_function = std::function<void(std::function<void(T)>)>;

  T await_resume() noexcept {
    return std::move(result_);
  }

  T result_;
};

template <>
struct callback_awaiter_base<void> {
  using callback_function = std::function<void(std::function<void()>)>;

  void await_resume() noexcept {}
};

template <typename T>
struct callback_awaiter : callback_awaiter_base<T> {
  using callback_function = callback_awaiter_base<T>::callback_function;
  callback_function callback_function_;

  explicit callback_awaiter(callback_function callback) : callback_function_(std::move(callback)) {}
  callback_awaiter(callback_awaiter&&) = default;

  bool await_ready() const noexcept {
    return false;
  }

  template <typename Promise>
  void await_suspend(std::coroutine_handle<Promise> handle) {
    if constexpr (std::is_void_v<T>) {
      callback_function_([handle, executor = handle.promise().executor_]() {
        executor->dispatch([handle] {
          handle.resume();
        });
      });
    } else {
      callback_function_([handle, this, executor = handle.promise().executor_](T value) {
        executor->dispatch([handle, this, value = std::move(value)]() mutable {
          this->result_ = std::move(value);
          handle.resume();
        });
      });
    }
  }
};

template <typename T>
using async = awaitable<T>;

template <typename T>
auto co_spawn(executor& executor, T&& coro) {
  coro.detach(executor);
  return coro;
}

}  // namespace coro
