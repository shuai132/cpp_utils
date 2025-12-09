#include <condition_variable>
#include <coroutine>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>

/// compiler check and debug config
#if defined(DEBUG_CORO_PROMISE_LEAK)
#include <cstdio>
#include <unordered_set>
struct debug_coro_promise {
  inline static std::unordered_set<void*> debug_coro_leak;
  static void dump() {
    printf("debug: debug_coro_leak.size: %zu\n", debug_coro_leak.size());
  }

  void* operator new(std::size_t size) {
    void* ptr = std::malloc(size);
    printf("new: %p, size: %zu\n", ptr, size);
    debug_coro_leak.insert(ptr);
    return ptr;
  }

  void operator delete(void* ptr, std::size_t size) {
    printf("free: %p, size: %zu\n", ptr, size);
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

  void unhandled_exception() noexcept {}

  T get_value() const {
    return value_.value();
  }

  std::optional<T> value_;
};

template <>
struct awaitable_promise_value<void> {
  void return_void() noexcept {}
  void unhandled_exception() noexcept {}
  void get_value() const {}
};

template <typename T>
struct final_awaitable {
  awaitable_promise<T>* parent;

  bool await_ready() noexcept {
    return !parent->continuation_;
  }

  std::coroutine_handle<> await_suspend(std::coroutine_handle<awaitable_promise<T>> h) noexcept {
    return h.promise().continuation_;
  }

  void await_resume() noexcept {
    parent->get_value();
  }
};

template <typename T>
struct awaitable_promise : awaitable_promise_value<T>, debug_coro_promise {
  std::suspend_always initial_suspend() {
    return {};
  }

  awaitable<T> get_return_object();

  final_awaitable<T> final_suspend() noexcept {
    return final_awaitable<T>{this};
  }

  std::coroutine_handle<> continuation_;
  executor* executor_ = nullptr;
};

template <typename T>
struct awaitable {
  using promise_type = awaitable_promise<T>;

  explicit awaitable(std::coroutine_handle<promise_type> h) : current_coro_handle_(h) {}
  awaitable(awaitable&& other) noexcept : current_coro_handle_(other.current_coro_handle_) {
    other.current_coro_handle_ = nullptr;
  }
  awaitable& operator=(awaitable&& other) noexcept {
    if (this != &other) {
      if (current_coro_handle_) current_coro_handle_.destroy();
      current_coro_handle_ = other.current_coro_handle_;
      other.current_coro_handle_ = nullptr;
    }
    return *this;
  }
  ~awaitable() {
    if (current_coro_handle_) {
      if (current_coro_handle_.done()) {
        current_coro_handle_.destroy();
      }
    }
  }
  awaitable(const awaitable&) = delete;
  awaitable(awaitable&) = delete;
  awaitable& operator=(const awaitable&) = delete;
  awaitable& operator=(awaitable&) = delete;

  bool await_ready() const noexcept {
    return false;
  }

  template <typename Promise>
  auto await_suspend(std::coroutine_handle<Promise> h) {
    auto& promise = current_coro_handle_.promise();
    promise.executor_ = h.promise().executor_;
    promise.continuation_ = h;
    return current_coro_handle_;
  }

  T await_resume() {
    return current_coro_handle_.promise().get_value();
  }

  std::coroutine_handle<promise_type> current_coro_handle_;
};

template <typename T>
awaitable<T> awaitable_promise<T>::get_return_object() {
  return awaitable<T>{std::coroutine_handle<awaitable_promise<T>>::from_promise(*this)};
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
      callback_function_([handle]() {
        auto executor = handle.promise().executor_;
        executor->dispatch([handle] {
          handle.resume();
        });
      });
    } else {
      callback_function_([handle, this](T value) {
        this->result_ = std::move(value);
        auto executor = handle.promise().executor_;
        executor->dispatch([handle] {
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
  coro.current_coro_handle_.promise().executor_ = &executor;
  coro.current_coro_handle_.resume();
  return coro;
}

}  // namespace coro
