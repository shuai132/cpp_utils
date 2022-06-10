#pragma once

#include <chrono>
#include <cstdint>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <functional>

/**
 * 用于精细的代码耗时分析
 * 为了方便调用 推荐临时定义如下宏
 * #define TRACK(msg)  perfTracker.track(std::string("L:") + std::to_string(__LINE__) + "\t"#msg)
 */
template<typename T = std::chrono::milliseconds>
class PerfTracker {
    using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

public:
    explicit PerfTracker(size_t capacity = 64) {
        points_.reserve(capacity);
        start_ = std::chrono::steady_clock::now();
    }

    PerfTracker(const PerfTracker &) = delete;
    void operator=(const PerfTracker &) = delete;
    PerfTracker(PerfTracker &&) noexcept = default;

    ~PerfTracker() {
        auto result = dump();
        if (dumpHandle) {
            dumpHandle(std::move(result));
        }
        else {
            std::cout << result << std::flush;
        }
    }

    inline uint64_t duration(TimePoint end, TimePoint start) {
        return std::chrono::duration_cast<T>(end - start).count();
    }

    void track(std::string tag) {
        points_.emplace_back(std::move(tag), std::chrono::steady_clock::now());
    }

private:
    std::string dump() {
        TimePoint end = std::chrono::steady_clock::now();
        std::ostringstream stream;
        stream << "PerfTracker Report begin:\n";
        stream << "total duration: " << duration(end, start_) << '\n';
        TimePoint lastTime = start_;
        for (auto &item: points_) {
            stream << "-> from last: " << duration(item.second, lastTime)
                   << ", from start: " << duration(item.second, start_)
                   << "\ttag: " << item.first << '\n';
            lastTime = item.second;
        }
        stream << "PerfTracker Report end\n";
        return stream.str();
    }

public:
    std::function<void(std::string result)> dumpHandle;

private:
    std::vector<std::pair<std::string, TimePoint>> points_;
    TimePoint start_;
};
