#pragma once

#include <functional>

#include "noncopyable.hpp"

template <typename T>
class Reference : noncopyable {
 public:
  void retain() {
    count_++;
    if (count_ == 1) {
      t_ = new T;
    }
  }

  void release() {
    if (count_ < 1) {
      return;
    }
    count_--;
    if (count_ == 0) {
      delete t_;
      t_ = nullptr;
    }
  }

  void reset() {
    if (count_ > 0) {
      count_ = 0;
      delete t_;
      t_ = nullptr;
    }
  }

  inline T *get() {
    return t_;
  }

  inline T *operator->() {
    if (count_ <= 0) {
      throw std::runtime_error("Reference lifecycle error");
    }
    return t_;
  }

 private:
  T *t_ = nullptr;
  int count_ = 0;
};
