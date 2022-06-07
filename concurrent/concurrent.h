#pragma once

#include <mutex>
#include <functional>
#include <shared_mutex>

#include "copy_on_write.h"

enum class Type {
    Default,// mutex lock
    RW,     // read write lock (need >= c++14)
    COW,    // copy on write
};

/**
 * Thread Safe Template
 *
 * @tparam T
 */
template<typename T, Type type = Type::Default, typename MutexType = std::mutex>
class concurrent;

namespace detail {

template<typename T>
class concurrent_base {
public:
    template<typename ...Args>
    explicit concurrent_base(Args&& ...args): data_(std::forward<Args>(args)...){}

    template<typename E>
    concurrent_base(std::initializer_list<E> il): data_(std::move(il)){}

public:
    // noncopyable
    concurrent_base(const concurrent_base&) = delete;
    const concurrent_base& operator=(const concurrent_base&) = delete;

public:
    T data_;
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
class concurrent<T, Type::Default, MutexType> : public detail::concurrent_base<T> {
private:
    using W = detail::Wrapper<T, MutexType>;

public:
    using detail::concurrent_base<T>::concurrent_base;

    inline W operator->() {
        return W{mutex_, data_};
    }

    inline void lock(const std::function<void(T*)>& op) {
        std::lock_guard<MutexType> lock(mutex_);
        op(&data_);
    }

private:
    T&data_ = detail::concurrent_base<T>::data_;
    MutexType mutex_;
};

#if (__cplusplus >= 201402L)

template<typename T, typename MutexType>
class concurrent<T, Type::RW, MutexType> : public detail::concurrent_base<T> {
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
    using detail::concurrent_base<T>::concurrent_base;

    inline WR operator->() {
        return WR{mutex_, data_};
    }

    inline void lockRead(const std::function<void(T*)>& op) {
        std::shared_lock<std::shared_timed_mutex> lock(mutex_);
        op(&data_);
    }

    inline void lockWrite(const std::function<void(T*)>& op) {
        std::lock_guard<std::shared_timed_mutex> lock(mutex_);
        op(&data_);
    }

    inline WR read() {
        return WR{mutex_, data_};
    }

    inline WW write() {
        return WW{mutex_, data_};
    }

private:
    T&data_ = detail::concurrent_base<T>::data_;
    std::shared_timed_mutex mutex_;
};

#endif

template <typename T>
class concurrent<T, Type::COW> : public copy_on_write<T> {};
