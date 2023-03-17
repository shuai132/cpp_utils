#pragma once

#include "SemaphoreSystemV.hpp"

/**
 * SystemV实现的进程锁
 * 当进程未释放锁而意外终止时，操作系统将自动释放。
 */
class MutexSystemV {
 public:
  explicit MutexSystemV(key_t key) : sem_(key, 1) {}

  MutexSystemV(const MutexSystemV &) = delete;
  void operator=(const MutexSystemV &) = delete;
  MutexSystemV(MutexSystemV &&) = default;

 public:
#if __linux__
  bool lock(uint32_t timeoutMs = 0) const {
    return sem_.p(timeoutMs);
  }
#else
  bool lock() const {
    return sem_.p();
  }
#endif
  void unlock() const {
    sem_.v();
  }

  int native_handle() const {
    return sem_.native_handle();
  }

 private:
  SemaphoreSystemV sem_;
};
