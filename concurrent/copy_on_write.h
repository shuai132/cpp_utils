#pragma once

#include <memory>
#include <mutex>
#include <functional>
#include <atomic>

/**
 * Copy On Write (COW) container wrapper implementation
 * support multi-thread read and write operator on container
 * Note: should *not* modify data on readOp
 *
 * @tparam T should support deep copy, can be std::vector<T>, std::list<T>, std::map<K,V>...
 */
template<typename T>
class copy_on_write {
public:
    using ST = std::shared_ptr<T>;

public:
    template<typename ...Args>
    explicit copy_on_write(Args&& ...args):_data(std::make_shared<T>(std::forward<Args>(args)...)){}

    template<typename E>
    copy_on_write(std::initializer_list<E> il):_data(std::make_shared<T>(il)){}

    void lockWrite(const std::function<void(const ST&)>& op) {
        ST d = std::make_shared<T>();
        std::lock_guard<std::mutex> lock(_mutex);
        *d = *_data; // deep copy
        op(d);
        std::atomic_store(&_data, d);
    }

    inline void lockRead(const std::function<void(ST)>& op) {
        auto tmp = std::atomic_load(&_data); // ensure use_count++ and atomic
        op(std::move(tmp));
    }

    inline ST operator->() {
        return std::atomic_load(&_data);
    }

public:
    // noncopyable
    copy_on_write(const copy_on_write&) = delete;
    const copy_on_write& operator=(const copy_on_write&) = delete;

private:
    ST _data = std::make_shared<T>();
    std::mutex _mutex;
};
