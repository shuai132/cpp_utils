#pragma once

#include <semaphore.h>
#include <sys/fcntl.h>

/**
 * Posix实现的进程锁
 * 当进程未释放锁而意外终止时，锁将无法释放。
 */
class MutexPosix {
 public:
  explicit MutexPosix(const char *name) {
    sem_ = sem_open(name, O_CREAT | O_RDWR, 0644, 1);
  }

  ~MutexPosix() {
    sem_close(sem_);
  }

  MutexPosix(const MutexPosix &) = delete;
  void operator=(const MutexPosix &) = delete;
  MutexPosix(MutexPosix &&) = default;

 public:
  void lock() const {
    sem_wait(sem_);
  }

  void unlock() const {
    sem_post(sem_);
  }

 private:
  sem_t *sem_;
};
