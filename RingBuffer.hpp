#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>

template <typename T, std::size_t N>
class RingBuffer {
 public:
  using value_type = T;
  using size_type = std::size_t;

  bool empty() const {
    return size_ == 0;
  }

  bool full() const {
    return size_ == N;
  }

  size_type size() const {
    return size_;
  }

  void push_back(value_type value) {
    if (full()) {
      pop_front();
    }

    buffer_[tail_] = std::move(value);
    tail_ = (tail_ + 1) % N;
    ++size_;
  }

  void pop_front() {
    if (empty()) {
      return;
    }

    {  // move to tmp and destroy it
      auto tmp = std::move(front());
    }

    head_ = (head_ + 1) % N;
    --size_;
  }

  value_type& front() const {
    return const_cast<value_type&>(buffer_[head_]);
  }

  value_type& back() const {
    return const_cast<value_type&>(buffer_[(tail_ + N - 1) % N]);
  }

 private:
  value_type buffer_[N];
  size_type head_ = 0;
  size_type tail_ = 0;
  size_type size_ = 0;
};
