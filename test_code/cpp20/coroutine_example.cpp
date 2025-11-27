#include <coroutine>
#include <iostream>
#include <thread>

struct Task {
  struct promise_type {
    Task get_return_object() {
      return {std::coroutine_handle<promise_type>::from_promise(*this)};
    }
    std::suspend_never initial_suspend() {
      return {};
    }
    std::suspend_never final_suspend() noexcept {
      return {};
    }
    void return_void() {}
    void unhandled_exception() {}
  };
  std::coroutine_handle<promise_type> handle_;
};

struct MyAwaiter {
  bool await_ready() {
    return false;
  }

  void await_suspend(std::coroutine_handle<Task::promise_type> handle) {
    std::cout << "协程暂停了，handle 指向整个协程\n";
    std::cout << "协程是否完成: " << handle.done() << "\n";

    // 在新线程中恢复协程
    std::thread([handle]() mutable {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::cout << "准备恢复协程...\n";
      handle.resume();  // 恢复整个协程，从 co_await 后继续
    }).detach();
  }

  void await_resume() {
    std::cout << "协程恢复执行\n";
  }
};

Task example_coroutine() {
  std::cout << "协程开始执行\n";

  int local_var = 42;
  std::cout << "局部变量 = " << local_var << "\n";

  co_await MyAwaiter{};  // 在这里暂停

  // handle 恢复时，会从这里继续执行
  // 所有局部变量状态都被保持
  std::cout << "协程恢复后，局部变量 = " << local_var << "\n";
  std::cout << "协程执行完毕\n";
}

int main() {
  example_coroutine();
  std::cout << "主线程继续执行\n";
  std::this_thread::sleep_for(std::chrono::seconds(2));
  return 0;
}
