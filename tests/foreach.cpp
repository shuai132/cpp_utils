#include "foreach.hpp"

#include <cstdio>
#include <vector>

int main() {
  FOR(i, 10) {
    printf("%d ", i);
  }
  printf("\n");

  std::vector<int> arrays{1, 2, 3};
  foreach (arrays, [](int& v) {
    printf("%d ", v);
    v++;
  })
    ;
  printf("\n");
  foreach (arrays, [](int v) {
    printf("%d ", v);
  })
    ;

  return 0;
}
