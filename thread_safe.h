#pragma once

#include <mutex>
#include <functional>

enum class Type {
    Default,
    ReadWrite,
};

/**
 * Thread Safe Template
 *
 * @tparam T
 */
template<typename T, Type type = Type::Default>
class thread_safe {};

namespace detail {

template<typename T>
class thread_safe_base {
public:
    template<typename ...Args>
    explicit thread_safe_base(Args&& ...args):_data(std::forward<Args>(args)...){}

    template<typename E>
    thread_safe_base(std::initializer_list<E> il):_data(il){}

public:
    // noncopyable
    thread_safe_base(const thread_safe_base&) = delete;
    const thread_safe_base& operator=(const thread_safe_base&) = delete;

public:
    T _data;
};

template<typename T>
struct Wrapper {
    explicit Wrapper(std::mutex& lock, T& data): lock(lock), data(data){}
    std::unique_lock<std::mutex> lock;
    T& data;
    inline T* operator->() {
        return &data;
    }
};

}

/**
 *
 * @tparam T
 */
template<typename T>
class thread_safe<T, Type::Default> : public detail::thread_safe_base<T> {
private:
    using W = detail::Wrapper<T>;

public:
    using detail::thread_safe_base<T>::thread_safe_base;

    inline W operator->() {
        return W{_mutex, _data};
    }

    inline void lock(const std::function<void(T*)>& op) {
        std::unique_lock<std::mutex> lock(_mutex);
        op(&_data);
    }

private:
    T& _data = detail::thread_safe_base<T>::_data;
    std::mutex _mutex;
};

template<typename T>
class thread_safe<T, Type::ReadWrite> : public detail::thread_safe_base<T> {
private:
    using W = detail::Wrapper<T>;

public:
    using detail::thread_safe_base<T>::thread_safe_base;

    inline W operator->() {
        return W{_mutexRead, _data};
    }

    inline void lockRead(const std::function<void(T*)>& op) {
        std::unique_lock<std::mutex> lock(_mutexRead);
        op(&_data);
    }

    inline void lockWrite(const std::function<void(T*)>& op) {
        std::unique_lock<std::mutex> lock(_mutexWrite);
        op(&_data);
    }

    inline W read() {
        return W{_mutexRead, _data};
    }

    inline W write() {
        return W{_mutexWrite, _data};
    }

private:
    T& _data = detail::thread_safe_base<T>::_data;
    std::mutex _mutexRead;
    std::mutex _mutexWrite;
};
