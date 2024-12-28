#include <atomic>
#include <cstdio>
#include <thread>

#include "libs/test_lib_a.h"
#include "libs/test_lib_so.h"

void some_func() {
  std::atomic_thread_fence(std::memory_order_seq_cst);
}

__attribute__((noinline)) void some_func_noinline() {}

int main() {
  static bool running = true;
  static size_t counter = 0;
  static std::shared_ptr<int> ptr;
  static std::unique_ptr<int> ptr_u;

  std::thread thread1([] {
    printf("thread1: start\n");
    while (running) {
      ++counter;  /// 这句不会被执行 因为编译器会优化掉整个循环

      /**
       * 执行一些具有`同步原语`副作用的代码 才会使编译器得知需要重新读取running 进而不会优化掉整个循环
       * 1. 内存栅栏
       * 2. 系统调用
       * 3. 原子变量操作（智能指针赋值也是这类）
       * 4. 包含上述操作的函数
       * 5. 调用库函数（动态库、静态库）。注意：当前编译单元内的函数没有这个待遇，无论是否被内联。
       * 6. unique_ptr内部一般也有一些内存栅栏 取决于编译器实现
       * 上面本质上都是内存栅栏
       */
      // std::atomic_thread_fence(std::memory_order_seq_cst);  // memory_order_acq_rel也可以
      // std::this_thread::sleep_for(std::chrono::milliseconds(1));
      // sleep(0);
      // printf("\n");
      // ptr = nullptr;
      // some_func();
      // test_lib_so_void();
      // test_lib_a_void();
      ptr_u = nullptr;
    }
    printf("thread1: should print after 1s\n");
  });

  std::thread thread2([] {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    running = false;
  });

  thread1.join();
  thread2.join();

  printf("%zu", counter);

  return 0;
}
