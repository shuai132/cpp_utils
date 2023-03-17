#pragma once

#include <semaphore.h>
#include <sys/fcntl.h>
#include <sys/time.h>

#include "sem_helper.hpp"

/**
 * Posix实现的进程锁
 * 当进程未释放锁而意外终止时，锁将无法释放。
 */
class MutexPosix {
 public:
  explicit MutexPosix(const char* name) {
    sem_ = sem_open(name, O_CREAT | O_RDWR, 0644, 1);
  }

  ~MutexPosix() {
    sem_close(sem_);
  }

  MutexPosix(const MutexPosix&) = delete;
  void operator=(const MutexPosix&) = delete;
  MutexPosix(MutexPosix&&) = default;

 public:
#if __linux__
  bool lock(uint32_t timeoutMs = 0) const {
    bool ret;
    if (timeoutMs > 0) {
      struct timespec timespec;  // NOLINT
      struct timeval now;        // NOLINT
      gettimeofday(&now, nullptr);
      long nsec = now.tv_usec * 1000 + (timeoutMs % 1000) * 1000000;
      timespec.tv_nsec = nsec % 1000000000;
      timespec.tv_sec = now.tv_sec + nsec / 1000000000 + timeoutMs / 1000;

      ret = (sem_timedwait(sem_, &timespec) == 0);
    } else {
      ret = (sem_wait(sem_) == 0);
    }
    return ret;
  }

  template <typename T = void>  // template for `link on use`
  void reset() {
    sem_helper::set_value(native_handle(), 1);
  }
#else
  void lock() const {
    sem_wait(sem_);
  }
#endif

  void unlock() const {
    sem_post(sem_);
  }

  sem_t* native_handle() {
    return sem_;
  }

 private:
  sem_t* sem_;
};
