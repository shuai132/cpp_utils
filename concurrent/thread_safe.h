#pragma once

#include <mutex>
#include <functional>
#include <shared_mutex>

#include "copy_on_write.h"

enum class Type {
    Default,
    ReadWrite,
    CopyOnWrite,
};

/**
 * Thread Safe Template
 *
 * @tparam T
 */
template<typename T, Type type = Type::Default, typename MutexType = std::mutex>
class thread_safe;

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

template<typename T, typename MutexType>
struct Wrapper {
    explicit Wrapper(MutexType& lock, T& data): lock(lock), data(data){}
    std::unique_lock<MutexType> lock;
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
template<typename T, typename MutexType>
class thread_safe<T, Type::Default, MutexType> : public detail::thread_safe_base<T> {
private:
    using W = detail::Wrapper<T, MutexType>;

public:
    using detail::thread_safe_base<T>::thread_safe_base;

    inline W operator->() {
        return W{_mutex, _data};
    }

    inline void lock(const std::function<void(T*)>& op) {
        std::lock_guard<MutexType> lock(_mutex);
        op(&_data);
    }

private:
    T& _data = detail::thread_safe_base<T>::_data;
    MutexType _mutex;
};

template<typename T, typename MutexType>
class thread_safe<T, Type::ReadWrite, MutexType> : public detail::thread_safe_base<T> {
private:
    struct WrapperRead {
        explicit WrapperRead(std::shared_timed_mutex& lock, T& data): lock(lock), data(data){}
        std::shared_lock<std::shared_timed_mutex> lock;
        T& data;
        inline T* operator->() {
            return &data;
        }
    };
    struct WrapperWrite {
        explicit WrapperWrite(std::shared_timed_mutex& lock, T& data): lock(lock), data(data){}
        std::unique_lock<std::shared_timed_mutex> lock;
        T& data;
        inline T* operator->() {
            return &data;
        }
    };
    using WR = WrapperRead;
    using WW = WrapperWrite;

public:
    using detail::thread_safe_base<T>::thread_safe_base;

    inline WR operator->() {
        return WR{_mutex, _data};
    }

    inline void lockRead(const std::function<void(T*)>& op) {
        std::shared_lock<std::shared_timed_mutex> lock(_mutex);
        op(&_data);
    }

    inline void lockWrite(const std::function<void(T*)>& op) {
        std::lock_guard<std::shared_timed_mutex> lock(_mutex);
        op(&_data);
    }

    inline WR read() {
        return WR{_mutex, _data};
    }

    inline WW write() {
        return WW{_mutex, _data};
    }

private:
    T& _data = detail::thread_safe_base<T>::_data;
    std::shared_timed_mutex _mutex;
};

template <typename T>
class thread_safe<T, Type::CopyOnWrite> : public copy_on_write<T> {};
