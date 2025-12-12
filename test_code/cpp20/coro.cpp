#define DEBUG_CORO_PROMISE_LEAK

#include "coro/coro.hpp"

#include <thread>

#include "TimeCount.hpp"
#include "assert_def.h"
#include "log.h"

struct BigObject {
  uint8_t data[1024 * 1024 * 1]{};
};

using namespace coro;

callback_awaiter<void> delay_ms(int delay_ms) {
  return callback_awaiter<void>([delay_ms](auto callback) {
    std::thread([delay_ms, callback = std::move(callback)] {
      std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
      callback();
    }).detach();
  });
}

async<int> coro_fun() {
  {
    LOG("delay_ms begin");
    TimeCount t;
    co_await delay_ms(500);
    ASSERT(t.elapsed() >= 500);
    ASSERT(std::abs(int(t.elapsed() - 500)) < 100);
    LOG("delay_ms end: %llu", t.elapsed());
  }

  LOG("callback_awaiter begin");
  auto ret = co_await callback_awaiter<int>([&](auto callback) {
    std::thread([callback = std::move(callback)] {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      callback(123);
    }).detach();
  });
  ASSERT(ret == 123);
  LOG("callback_awaiter end");
  co_return ret;
}

async<void> coro_task() {
  BigObject o;
  o.data[1024] = 1;
  int ret = co_await coro_fun();
  LOG("coro_test: %d, %d", ret, o.data[1024]);
  ASSERT(ret == 123);
  ASSERT(o.data[1024] == 1);
}

async<void> loop_task(const char* tag, int ms) {
  int count = 3;
  while (count--) {
    TimeCount t;
    co_await delay_ms(ms);
    LOG("%s: %d, elapsed: %llu", tag, ms, t.elapsed());
    ASSERT(t.elapsed() >= ms);
    ASSERT(std::abs(int(t.elapsed() - ms)) < 100);
  }
}

void debug_and_stop(executor& executor, int wait_ms = 1000) {
  std::thread([&executor, wait_ms] {
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms));
    executor.post([&executor] {
#ifdef DEBUG_CORO_PROMISE_LEAK
      debug_coro_promise::dump();
      ASSERT(debug_coro_promise::debug_coro_leak.empty());
#endif
      executor.stop();
    });
  }).detach();
}

void test_coro(executor& executor) {
  co_spawn(executor, loop_task("A", 100));
  co_spawn(executor, loop_task("B", 200));
  co_spawn(executor, loop_task("C", 300));

  co_spawn(executor, coro_task());
  // or: coro_task().detach(executor);

  coro_task().detach_with_callback(executor, [] {
    LOG("coro_task finished");
  });
}

void test_simple(executor& executor) {
  co_spawn(executor, []() -> async<void> {
    TimeCount t;
    const auto ms = 100;
    co_await delay_ms(ms);
    LOG("%s: %d, elapsed: %llu", "test_simple", ms, t.elapsed());
    ASSERT(t.elapsed() >= ms);
    ASSERT(std::abs(int(t.elapsed() - ms)) < 100);
  }());
}

int main() {
  LOG("init");
  executor executor;
  test_coro(executor);
  test_simple(executor);
  debug_and_stop(executor);
  LOG("run_loop...");
  executor.run_loop();
  return 0;
}
