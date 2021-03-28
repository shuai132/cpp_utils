#pragma once

#include <mutex>
#include <functional>

enum class Type {
    Default,
    ReadWrite,
};

enum class LockType {
    Read,
    Write,
};

/**
 * Thread Safe Template
 *
 * @tparam T
 */
template<typename T, Type type = Type::Default>
class thread_safe {
private:
    struct W {
        explicit W(std::mutex& lock, T& data):lock(lock), data(data){}
        std::unique_lock<std::mutex> lock;
        T& data;
        inline T* operator->() {
            return &data;
        }
    };

public:
    template<typename ...Args>
    explicit thread_safe(Args&& ...args):_data(std::forward<Args>(args)...){}

    template<typename E>
    thread_safe(std::initializer_list<E> il):_data(il){}

    inline W operator->() {
        return W{_mutex, _data};
    }

    template <typename TT=void>
    inline
    typename std::enable_if<type==Type::Default, TT>::type
    lock(const std::function<void(T*)>& op) {
        std::unique_lock<std::mutex> lock(_mutex);
        op(&_data);
    }

    template <typename R=void>
    inline
    typename std::enable_if<type==Type::ReadWrite, R>::type
    lock(LockType lockType, const std::function<void(T*)>& op) {
        std::unique_lock<std::mutex> lock(lockType == LockType::Read ? _mutexRead : _mutexWrite);
        op(&_data);
    }

    template <typename R=W>
    inline
    typename std::enable_if<type==Type::ReadWrite, R>::type
    read() {
        return W{_mutexRead, _data};
    }

    template <typename R=W>
    inline
    typename std::enable_if<type==Type::ReadWrite, R>::type
    write() {
        return W{_mutexWrite, _data};
    }

public:
    // noncopyable
    thread_safe(const thread_safe&) = delete;
    const thread_safe& operator=(const thread_safe&) = delete;

private:
    std::mutex _mutex;
    std::mutex _mutexRead;
    std::mutex _mutexWrite;
    T _data;
};
