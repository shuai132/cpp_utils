#undef __cplusplus
#define __cplusplus 201103L
#include "make_unique.hpp"

#include <cstdio>

int main() {
  auto value = std::make_unique<int>(123);
  printf("%d", *value);
  return 0;
}
