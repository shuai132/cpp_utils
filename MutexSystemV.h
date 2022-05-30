#pragma once

#include <sys/sem.h>

/**
 * SystemV实现的进程锁
 * 当进程未释放锁而意外终止时，操作系统将自动释放。
 */
class MutexSystemV {
public:
    explicit MutexSystemV(key_t key) {
        id_ = semget(key, 1, IPC_CREAT | IPC_EXCL | 0644);
        if (id_ == -1) {
            id_ = semget(key, 0, 0);
        } else {
            semctl(id_, 0, SETVAL, 1);
        }
    }

    ~MutexSystemV() {
        semctl(id_, 0, IPC_RMID, 0);
    }

    MutexSystemV(const MutexSystemV&) = delete;
    void operator=(const MutexSystemV&) = delete;
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
