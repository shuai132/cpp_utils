#include "defer.hpp"

#include <cstdio>

int main() {
  defer {
    printf("1\n");
  };
  defer {
    printf("2\n");
  };
  return 0;
}
