#pragma once

#include <chrono>
#include <cstdint>
#include <vector>
#include <string>

/**
 * 用于精细的代码耗时分析
 * 为了方便调用 推荐临时定义如下宏
 * #define TRACK(msg)  perfTracker.track(std::string("L:") + std::to_string(__LINE__) + "\t"#msg)
 */
class PerfTracker {
    using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

public:
    explicit PerfTracker(size_t capacity = 64) {
        points_.reserve(capacity);
        start_ = std::chrono::steady_clock::now();
    }

    PerfTracker(const PerfTracker &) = delete;
    void operator=(const PerfTracker &) = delete;
    PerfTracker(PerfTracker &&) = default;

    ~PerfTracker() {
      dump();
    }

    template<typename T>
    inline uint64_t duration(TimePoint end, TimePoint start) {
        return std::chrono::duration_cast<T>(end - start).count();
    }

    void track(std::string tag) {
        points_.emplace_back(std::move(tag), std::chrono::steady_clock::now());
    }

    template<typename T = std::chrono::milliseconds>
    void dump() {
        TimePoint end = std::chrono::steady_clock::now();
        printf("PerfTracker Report begin:\n");
        printf("total duration: %llu\n", duration<T>(end, start_));
        TimePoint lastTime = start_;
        for (auto &item: points_) {
            printf("-> from last: %llu, from start: %llu\ttag: %s\n",
                   duration<T>(item.second, lastTime),
                   duration<T>(item.second, start_),
                   item.first.c_str());
            lastTime = item.second;
        }
        printf("PerfTracker Report end\n");
    }

private:
    std::vector<std::pair<std::string, TimePoint>> points_;
    TimePoint start_;
};
