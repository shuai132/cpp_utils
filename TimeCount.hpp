#pragma once

#include <chrono>
#include <cstdint>

template <typename Duration = std::chrono::milliseconds>
class TimeCount {
 public:
  explicit TimeCount() = default;

  TimeCount(const TimeCount &) = delete;
  void operator=(const TimeCount &) = delete;
  TimeCount(TimeCount &&) noexcept = default;

  template <class D = std::chrono::milliseconds>
  uint64_t elapsed() const {
    return std::chrono::duration_cast<D>(std::chrono::steady_clock::now() - start_).count();
  }

  void reset() {
    start_ = std::chrono::steady_clock::now();
  }

 private:
  using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;
  TimePoint start_ = std::chrono::steady_clock::now();
};
