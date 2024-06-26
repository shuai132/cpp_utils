#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <vector>

#include "noncopyable.hpp"

namespace observer {

namespace detail {

using ObserverId = uint32_t;

struct Scheduler : noncopyable {
  using Callable = std::function<void()>;
  virtual ~Scheduler() = default;
  virtual void call(Callable callable) = 0;
};

struct ObserverBase {
  virtual void disconnect(ObserverId id) = 0;
  virtual ~ObserverBase() = default;
};

struct Disposable {
  ObserverId id;
  std::weak_ptr<detail::ObserverBase> ptr;

  template <class T>
  void addTo(T &dispose) const {
    dispose.add(*this);
  }

  void dispose() const {
    auto p = ptr.lock();
    if (p) {
      p->disconnect(id);
    }
  }
};

template <typename... Signature>
class ObserverImpl : noncopyable, public ObserverBase, public std::enable_shared_from_this<ObserverImpl<Signature...>> {
 public:
  using Func = std::function<void(Signature...)>;
  using SchedulerSp = std::shared_ptr<Scheduler>;

 public:
  Disposable connect(Func func, SchedulerSp scheduler = nullptr) {
    std::lock_guard<std::mutex> lock(observerMutex_);
    ObserverId id = observerId_++;
    connections_.emplace(id, std::make_pair(std::move(func), std::move(scheduler)));
    return {
        .id = id,
        .ptr = ObserverImpl::shared_from_this(),
    };
  }

  void disconnect(ObserverId key) override {
    std::lock_guard<std::mutex> lock(observerMutex_);
    connections_.erase(key);
  }

  void disconnect_all() {
    std::lock_guard<std::mutex> lock(observerMutex_);
    connections_.clear();
  }

  template <typename... Args>
  void emit(const Args &...args) {
    std::lock_guard<std::mutex> lock(observerMutex_);
    for (const auto &item : connections_) {
      auto &pair = item.second;
      if (pair.second == nullptr) {
        pair.first(args...);
      } else {
        pair.second->call([=] {
          pair.first(args...);
        });
      }
    }
  }

 private:
  ObserverId observerId_ = 0;
  std::map<ObserverId, std::pair<Func, SchedulerSp>> connections_;
  std::mutex observerMutex_;
};

}  // namespace detail

using detail::ObserverId;

using detail::Disposable;

using detail::Scheduler;

class Dispose : noncopyable {
 public:
  void add(Disposable observer) {
    std::lock_guard<std::mutex> lock(mutex_);
    observers_.emplace_back(std::move(observer));
  }

  void dispose() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto &item : observers_) {
      auto ptr = item.ptr.lock();
      if (ptr) {
        ptr->disconnect(item.id);
      }
    }
    observers_.clear();
  }

  void clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    observers_.clear();
  }

  ~Dispose() {
    dispose();
  }

 private:
  std::vector<Disposable> observers_;
  std::mutex mutex_;
};

template <typename... Args>
class Observer : noncopyable {
  using ObserverImpl = detail::ObserverImpl<Args...>;
  using ObserverImplSp = std::shared_ptr<ObserverImpl>;

 public:
  inline ObserverImplSp operator->() {
    return impl_;
  }

  inline void operator()(Args &&...args) {
    impl_->emit(std::forward<Args>(args)...);
  }

 private:
  ObserverImplSp impl_ = std::make_shared<ObserverImpl>();
};

}  // namespace observer
