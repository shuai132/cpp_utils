#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>

/**
 * Copy On Write (COW) container wrapper implementation
 * support multi-thread read and write operator on container
 * Note: should *not* modify data on lockRead and operator->
 *
 * @tparam T should support deep copy, can be std::vector<T>, std::list<T>, std::map<K,V>...
 */
template <typename T>
class copy_on_write {
 public:
  using ST = std::shared_ptr<T>;

 public:
  template <typename... Args>
  explicit copy_on_write(Args&&... args) : data_(std::make_shared<T>(std::forward<Args>(args)...)) {}

  template <typename E>
  copy_on_write(std::initializer_list<E> il) : data_(std::make_shared<T>(std::move(il))) {}

  void lockWrite(const std::function<void(const ST&)>& op) {
    std::lock_guard<std::mutex> lock(mutex_);
    ST d;
    // Only copy if there are other references
    if (data_.use_count() > 1) {
      d = std::make_shared<T>(*data_);  // deep copy
    } else {
      d = data_;  // Reuse existing data
    }
    op(d);
    std::atomic_store(&data_, d);
  }

  inline void lockRead(const std::function<void(ST)>& op) {
    auto tmp = std::atomic_load(&data_);  // ensure use_count++ and atomic
    op(std::move(tmp));
  }

  inline ST operator->() {
    return std::atomic_load(&data_);
  }

 public:
  // noncopyable
  copy_on_write(const copy_on_write&) = delete;
  const copy_on_write& operator=(const copy_on_write&) = delete;

 private:
  ST data_ = std::make_shared<T>();
  std::mutex mutex_;
};
