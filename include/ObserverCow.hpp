#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <vector>

#include "concurrent/copy_on_write.hpp"
#include "noncopyable.hpp"

namespace observer {

namespace detail {

using ObserverCowId = uint32_t;

struct Scheduler : noncopyable {
  using Callable = std::function<void()>;
  virtual ~Scheduler() = default;
  virtual void call(Callable callable) = 0;
};

struct ObserverCowBase {
  virtual void disconnect(ObserverCowId id) = 0;
  virtual ~ObserverCowBase() = default;
};

struct Disposable {
  ObserverCowId id;
  std::weak_ptr<detail::ObserverCowBase> ptr;

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
class ObserverCowImpl : noncopyable, public ObserverCowBase, public std::enable_shared_from_this<ObserverCowImpl<Signature...>> {
 public:
  using Func = std::function<void(Signature...)>;
  using SchedulerSp = std::shared_ptr<Scheduler>;

 public:
  Disposable connect(Func func, SchedulerSp scheduler = nullptr) {
    ObserverCowId id = observerId_.fetch_add(1, std::memory_order_relaxed);
    connections_.lockWrite([id, func = std::move(func), scheduler = std::move(scheduler)](const Conn &conn) {
      conn->emplace(id, std::make_pair(std::move(func), std::move(scheduler)));
    });
    return {
        .id = id,
        .ptr = ObserverCowImpl::shared_from_this(),
    };
  }

  void disconnect(ObserverCowId key) override {
    connections_.lockWrite([key](const Conn &conn) {
      conn->erase(key);
    });
  }

  void disconnect_all() {
    connections_.lockWrite([](const Conn &conn) {
      conn->clear();
    });
  }

  template <typename... Args>
  void emit(const Args &...args) {
    connections_.lockRead([&args...](const Conn &conn) {
      for (const auto &item : *conn) {
        auto &pair = item.second;
        if (pair.second == nullptr) {
          pair.first(args...);
        } else {
          pair.second->call([=] {
            pair.first(args...);
          });
        }
      }
    });
  }

 private:
  std::atomic<ObserverCowId> observerId_{0};
  copy_on_write<std::map<ObserverCowId, std::pair<Func, SchedulerSp>>> connections_;
  using Conn = typename decltype(connections_)::ST;
};

}  // namespace detail

using detail::ObserverCowId;

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
class ObserverCow : noncopyable {
  using ObserverCowImpl = detail::ObserverCowImpl<Args...>;
  using ObserverCowImplSp = std::shared_ptr<ObserverCowImpl>;

 public:
  inline ObserverCowImplSp operator->() {
    return impl_;
  }

  inline void operator()(Args &&...args) {
    impl_->emit(std::forward<Args>(args)...);
  }

 private:
  ObserverCowImplSp impl_ = std::make_shared<ObserverCowImpl>();
};

}  // namespace observer
