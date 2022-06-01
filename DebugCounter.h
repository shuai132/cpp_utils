#pragma once

#include <string>
#include <atomic>

class DebugCounter {
public:
    explicit DebugCounter(std::string name) : name_(std::move(name)) {};
    DebugCounter(const DebugCounter&) = delete;
    void operator=(const DebugCounter&) = delete;
    DebugCounter(DebugCounter &&) = delete;

    void inc() {
        ++count_;
        printf("%s: inc %d\n", name_.c_str(), count_.load());
    }
    void dec() {
        --count_;
        printf("%s: dec %d\n", name_.c_str(), count_.load());
    }

private:
    std::string name_;
    std::atomic<int32_t> count_{0};
};
