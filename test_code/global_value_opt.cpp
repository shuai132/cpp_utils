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
  static int* ptr_int;
  std::thread thread1([] {
    printf("thread1: start\n");
    while (running) {
      /**
       * 实际场景`running`应当用原子变量，这里为了测试非原子变量可能被编译器优化的风险。
       *
       * 使用内存栅栏能够阻止编译器优化（指令重排、CPU乱序执行），也能够保证`running`普通变量的可见性。
       * 另外，执行一些具有副作用的代码，也能做到这个效果。这取决于操作系统和编译器的具体实现，此处仅为学习研究。不可以依赖这种行为，以确保代码的健壮和可读性。
       * 1. 内存栅栏
       * 2. 系统调用
       * 3. 原子变量操作（智能指针赋值也是这类）
       * 4. 包含上述操作的函数
       * 5. 调用库函数（动态库、静态库）。注意：当前编译单元内的函数没有这个待遇，无论是否被内联。
       * 6. `unique_ptr`内部一般也有一些内存栅栏 取决于编译器实现
       */
      /// 阻止优化的一些方式：
      /// 虽然一些调用有这种副作用，但是实际上要是要明确的利用内存栅栏才是最规范的。
      std::atomic_thread_fence(std::memory_order_acquire);  // `std::memory_order_relaxed`不行，其他的在这里都能起到作用，尽管语义不恰当。
      // std::this_thread::sleep_for(std::chrono::milliseconds(1));
      // sleep(0);
      // printf("\n");
      // ptr = nullptr;
      // some_func();
      // test_lib_so_void();
      // test_lib_a_void();
      // ptr_u = nullptr;

      /// 有趣的是，这个能够阻止编译器优化掉整个循环，但是`running`不会被重新读取，导致死循环。要避免这种
      // ptr_int = new int();

      /// 业务代码
      ++counter;  /// 这句不会被执行 因为编译器会优化掉整个循环
    }
    printf("thread1: should print after 1s\n");
  });

  std::thread thread2([] {
    printf("thread2: start\n");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    running = false;
    std::atomic_thread_fence(std::memory_order_release);  // 这样是最规范的，配合上面的`std::atomic_thread_fence(std::memory_order_acquire)`
    printf("thread2: end\n");
  });

  thread1.join();
  thread2.join();

  printf("%zu", counter);

  return 0;
}
