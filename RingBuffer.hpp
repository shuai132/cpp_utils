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

    {  // move to tmp and destroy it, DO NOT use front().~T() here!
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

  class Iterator {
   public:
    Iterator(RingBuffer* buffer, size_type index, size_type count) : buffer_(buffer), index_(index), count_(count) {}

    value_type& operator*() const {
      return buffer_->buffer_[index_];
    }

    Iterator& next() {
      if (count_ > 0) {
        index_ = (index_ + 1) % N;
        --count_;
      }
      return *this;
    }

    bool operator!=(const Iterator& other) const {
      return index_ != other.index_ || count_ != other.count_;
    }

   private:
    RingBuffer* buffer_;
    size_type index_;
    size_type count_;
  };

  Iterator begin() {
    return Iterator(this, head_, size_);
  }

  Iterator end() {
    return Iterator(this, tail_, 0);
  }

 private:
  value_type buffer_[N];
  size_type head_ = 0;
  size_type tail_ = 0;
  size_type size_ = 0;
};
