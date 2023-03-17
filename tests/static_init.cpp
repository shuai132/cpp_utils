#include "static_init.hpp"

#include <cstdio>

static_init([] {
  printf("hello, ");
});

static_init([] {
  printf("world");
});

int main() {
  // static_init([]{}) // compile error
  return 0;
}
