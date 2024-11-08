#pragma once

#include <cstdio>

class Test {
 public:
  Test() {
    printf("Test default construct:\n");
  }
  explicit Test(int v) : v(v) {
    printf("Test construct: v: %d\n", v);
  }
  ~Test() {
    printf("~Test: v: %d\n", v);
  }
  Test(const Test& other) {
    printf("copy construct: %d\n", other.v);
    this->v = other.v;
  }
  Test(Test&& other) noexcept {
    printf("move construct: %d\n", other.v);
    this->v = other.v;
  }
  Test& operator=(Test&& other) noexcept {
    printf("move operator=: %d\n", other.v);
    this->v = other.v;
    return *this;
  }
  Test& operator=(const Test& other) noexcept {
    printf("copy operator=: %d\n", other.v);
    this->v = other.v;
    return *this;
  }
  int v = -1;
};
