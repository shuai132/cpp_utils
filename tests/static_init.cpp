#include "static_init.h"

static_init([] {
  printf("hello, ");
})

    static_init([] {
      printf("world");
    })

        int main() {
  //    static_init([]{}) // compile error
  return 0;
}
