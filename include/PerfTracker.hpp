#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

/**
 * 用于精细的代码耗时分析
 * 1. 为了方便调用 推荐定义如下宏
 * #define TRACK(msg)  perfTracker.track(std::string("L:") + std::to_string(__LINE__) + "\t"#msg)
 * 2. 避免性能损失 默认非线程安全
 */
template <bool ThreadSafe = false, typename Duration = std::chrono::milliseconds>
class PerfTracker {
  struct FakeMutex {
    void lock() {}
    void unlock() {}
  };

  using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

 public:
  explicit PerfTracker(bool autodump = true, size_t capacity = 64) : autodump_(autodump) {
    points_.reserve(capacity);
    start_ = std::chrono::steady_clock::now();
  }

  PerfTracker(const PerfTracker &) = delete;
  void operator=(const PerfTracker &) = delete;
  PerfTracker(PerfTracker &&) noexcept = default;

  ~PerfTracker() {
    if (autodump_) {
      dump();
    }
  }

  void track(std::string tag) {
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    points_.emplace_back(std::move(tag), std::chrono::steady_clock::now());
  }

  void dump() {
    auto result = genDump();
    if (dumpHandle) {
      dumpHandle(std::move(result));
    } else {
      std::cout << result << std::flush;
    }
  }

  template <class D = std::chrono::milliseconds>
  uint64_t elapsed() const {
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<D>(end - start_).count();
  }

  void reset() {
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    start_ = std::chrono::steady_clock::now();
    points_.clear();
  }

  std::string genDump() {
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    TimePoint end = std::chrono::steady_clock::now();
    std::ostringstream stream;
    stream << "PerfTracker Report begin:\n";
    stream << "total duration: " << duration(end, start_) << '\n';
    TimePoint lastTime = start_;
    for (auto &item : points_) {
      stream << "-> from last: " << duration(item.second, lastTime) << ", from start: " << duration(item.second, start_) << "\ttag: " << item.first
             << '\n';
      lastTime = item.second;
    }
    stream << "PerfTracker Report end\n";
    return stream.str();
  }

 private:
  inline uint64_t duration(TimePoint end, TimePoint start) const {
    return std::chrono::duration_cast<Duration>(end - start).count();
  }

 public:
  std::function<void(std::string result)> dumpHandle;

 private:
  std::vector<std::pair<std::string, TimePoint>> points_;
  TimePoint start_;
  typename std::conditional<ThreadSafe, std::mutex, FakeMutex>::type mutex_;
  bool autodump_;
};
