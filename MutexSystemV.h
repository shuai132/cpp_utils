#pragma once

#include <sys/sem.h>

#include <cerrno>
#include <stdexcept>

/**
 * SystemV实现的进程锁
 * 当进程未释放锁而意外终止时，操作系统将自动释放。
 */
class MutexSystemV {
 public:
  explicit MutexSystemV(key_t key) {
    id_ = semget(key, 1, IPC_CREAT | IPC_EXCL | 0644);
    if (id_ >= 0) {
      semctl(id_, 0, SETVAL, 1);
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

  ~MutexSystemV() {
    semctl(id_, 0, IPC_RMID, 0);
  }

  MutexSystemV(const MutexSystemV &) = delete;
  void operator=(const MutexSystemV &) = delete;
  MutexSystemV(MutexSystemV &&) = default;

 public:
  void lock() const {
    sembuf buf{0, -1, SEM_UNDO};
    semop(id_, &buf, 1);
  }

  void unlock() const {
    sembuf buf{0, +1, SEM_UNDO};
    semop(id_, &buf, 1);
  }

 private:
  int id_;
};
