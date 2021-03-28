#pragma once

#include <memory>
#include <mutex>
#include <functional>

/**
 * Thread Safe Template
 *
 * @tparam T
 */
template<typename T>
class thread_safe {
public:
    using ST = std::shared_ptr<T>;

private:
    struct W {
        explicit W(std::mutex& lock, ST& data):lock(lock), data(data){}
        std::unique_lock<std::mutex> lock;
        ST& data;
        inline ST& operator->() {
            return data;
        }
    };

public:
    template<typename ...Args>
    explicit thread_safe(Args&& ...args):_data(std::make_shared<T>(std::forward<Args>(args)...)){}

    template<typename E>
    thread_safe(std::initializer_list<E> il):_data(std::make_shared<T>(il)){}

    inline W operator->() {
        return W{_mutex, _data};
    }

    inline void lock(const std::function<void(const ST&)>& op) {
        std::unique_lock<std::mutex> lock(_mutex);
        op(_data);
    }

public:
    // noncopyable
    thread_safe(const thread_safe&) = delete;
    const thread_safe& operator=(const thread_safe&) = delete;

private:
    std::mutex _mutex;
    ST _data;
};
