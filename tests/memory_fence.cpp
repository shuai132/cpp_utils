#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex mutex;
uint32_t value_none = 0;
uint32_t value_mutex = 0;
uint32_t value_spin_lock = 0;
uint32_t value_spin_lock_relax = 0;
uint32_t value_memory_fence_1 = 0;
uint32_t value_memory_fence_2 = 0;
std::atomic<bool> spin_locked;
volatile bool volatile_spin_locked;
bool spin_locked_2;

/**
 * 不加锁 导致value的读写有交叉 以及线程间的可见性问题
 * @param count
 */
void test_none(uint32_t count) {
  for (uint32_t i = 0; i < count; ++i) {
    value_none++;
  }
}

/**
 * 加锁 同时也能保证value_mutex的可见性正确
 * @param count
 */
void test_mutex(uint32_t count) {
  for (uint32_t i = 0; i < count; ++i) {
    mutex.lock();
    value_mutex++;
    mutex.unlock();
  }
}

/**
 * 自旋锁会使得value_spin_lock的可见性也是正确的
 * @param count
 */
void test_spin_lock(uint32_t count) {
  for (uint32_t i = 0; i < count; ++i) {
    bool expected = false;
    while (!spin_locked.compare_exchange_weak(expected, true, std::memory_order_acquire)) {
      expected = false;
    }
    value_spin_lock++;
    spin_locked.store(false, std::memory_order_release);
  }
}

/**
 * 都使用std::memory_order_relaxed无法保证顺序一致性
 * 在x86平台 很可能没问题
 * 在arm平台 Debug编译时5%的概率会错误几次
 *          Release编译优化结果明显不符了
 * @param count
 */
void test_spin_lock_relax(uint32_t count) {
  for (uint32_t i = 0; i < count; ++i) {
    bool expected = false;
    while (!spin_locked.compare_exchange_weak(expected, true, std::memory_order_relaxed)) {
      expected = false;
    }
    value_spin_lock_relax++;
    spin_locked.store(false, std::memory_order_relaxed);
  }
}

/**
 * volatile bool无法实现自旋锁：
 * 1.无法保证 value 的可见性
 * 2.无法保证顺序一致性
 * 3.无法保证lock的原子性
 *
 * 在x86的环境下 很可能没问题 即使不加volatile
 * 在arm的macOS 结果不正确
 * @param count
 */
void test_memory_fence_1(uint32_t count) {
  for (uint32_t i = 0; i < count; ++i) {
    while (volatile_spin_locked) {
    }
    volatile_spin_locked = true;
    value_memory_fence_1++;
    volatile_spin_locked = false;
  }
}

/**
 * 普通的变量 也可以使用原子操作
 * 如改为__ATOMIC_RELAXED后 结果同test_spin_lock_relax
 * @param count
 */
void test_memory_fence_2(uint32_t count) {
#if defined(__linux__) || defined(__APPLE__)
  for (uint32_t i = 0; i < count; ++i) {
    bool expected = false;
    bool desire = true;
    while (!__atomic_compare_exchange(&spin_locked_2, &expected, &desire, true, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)) {
      expected = false;
    }
    value_memory_fence_2++;
    bool value = false;
    __atomic_store(&spin_locked_2, &value, __ATOMIC_RELEASE);
  }
#endif
}

int main() {
  static const int THREAD_NUM = 5;
  static const int COUNT = 100000;

  auto test = [](auto test_func) {
    std::thread threads[THREAD_NUM];
    for (std::thread& i : threads) {
      i = std::thread(test_func, COUNT);
    }
    for (std::thread& thread : threads) {
      thread.join();
    }
  };
  test(test_none);
  test(test_mutex);
  test(test_spin_lock);
  test(test_spin_lock_relax);
  test(test_memory_fence_1);
  test(test_memory_fence_2);

  std::cout << "value_none: " << value_none << std::endl;
  std::cout << "value_mutex: " << value_mutex << std::endl;
  std::cout << "value_spin_lock: " << value_spin_lock << std::endl;
  std::cout << "value_spin_lock_relax: " << value_spin_lock_relax << std::endl;
  std::cout << "value_memory_fence_1: " << value_memory_fence_1 << std::endl;
  std::cout << "value_memory_fence_2: " << value_memory_fence_2 << std::endl;

  return 0;
}
