#pragma once

#include <sys/sem.h>
#include <sys/time.h>

#include <cerrno>
#include <stdexcept>

/**
 * SystemV实现的信号量
 * 当进程未释放锁而意外终止时，操作系统将自动释放。
 */
class SemaphoreSystemV {
 public:
  explicit SemaphoreSystemV(key_t key, int value) {
    id_ = semget(key, 1, IPC_CREAT | IPC_EXCL | 0644);
    if (id_ >= 0) {
      semctl(id_, 0, SETVAL, value);
    } else if (errno == EEXIST) {
      id_ = semget(key, 0, 0);
      // make sure it's initialized
      for (;;) {
        semid_ds ds;  // NOLINT(cppcoreguidelines-pro-type-member-init)
        semctl(id_, 0, IPC_STAT, &ds);
        if (ds.sem_otime != 0) break;  // no need sleep
      }
    } else {
      throw std::runtime_error("semget error");
    }
  }

  ~SemaphoreSystemV() {
    semctl(id_, 0, IPC_RMID, 0);
  }

  SemaphoreSystemV(const SemaphoreSystemV &) = delete;
  void operator=(const SemaphoreSystemV &) = delete;
  SemaphoreSystemV(SemaphoreSystemV &&) = default;

 public:
#if __linux__
  bool p(uint32_t timeoutMs = 0) const {
    bool ret;
    if (timeoutMs > 0) {
      struct timespec waitTime;  // NOLINT
      waitTime.tv_nsec = (timeoutMs % 1000) * 1000000;
      waitTime.tv_sec = timeoutMs / 1000;

      sembuf buf{0, -1, SEM_UNDO};
      ret = (semtimedop(id_, &buf, 1, &waitTime) == 0);
    } else {
      sembuf buf{0, -1, SEM_UNDO};
      ret = (semop(id_, &buf, 1) == 0);
    }
    return ret;
  }
#else
  bool p() const {
    sembuf buf{0, -1, SEM_UNDO};
    semop(id_, &buf, 1);
    return true;
  }
#endif

  void v() const {
    sembuf buf{0, +1, SEM_UNDO};
    semop(id_, &buf, 1);
  }

  int native_handle() const {
    return id_;
  }

 private:
  int id_;
};
