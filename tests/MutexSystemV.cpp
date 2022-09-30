#include "MutexSystemV.h"

#include <unistd.h>

#include <cstdio>
#include <mutex>

int main() {
  auto test = [](const char *name) {
    auto id = ftok(".", 0);
    printf("id:0x%X\n", id);
    MutexSystemV mutex(id);
    std::lock_guard<MutexSystemV> lock(mutex);
    printf("%s: start: %d\n", name, getpid());
    sleep(1);
    printf("%s: end: %d\n", name, getpid());
  };

  auto pid = fork();
  if (pid == 0) {
    test("child");
  } else {
    test("parent");
    pause();
  }
  return 0;
}
