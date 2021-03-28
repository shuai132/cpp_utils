#pragma once

#include <mutex>
#include <functional>

/**
 * Thread Safe Template
 *
 * @tparam T
 */
template<typename T>
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

    inline void lock(const std::function<void(T*)>& op) {
        std::unique_lock<std::mutex> lock(_mutex);
        op(&_data);
    }

public:
    // noncopyable
    thread_safe(const thread_safe&) = delete;
    const thread_safe& operator=(const thread_safe&) = delete;

private:
    std::mutex _mutex;
    T _data;
};
