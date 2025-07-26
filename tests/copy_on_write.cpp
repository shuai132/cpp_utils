#include <cstdio>
#include <cstdlib>
#include <thread>
#include <vector>

#include "concurrent/concurrent.hpp"

#ifndef cpp_utils_DISABLE_DETAIL_PRINT
#define detail_printf printf
#else
#define detail_printf(...) ((void)0)
#endif

int main() {
  concurrent<std::vector<size_t>, Type::COW> l;
  // or
  // copy_on_write<std::vector<size_t>> l;
  using ST = decltype(l)::ST;

  const size_t WriteNum = 10000;
  volatile size_t deleteNum = 0;
  volatile size_t remainNum = 0;
  volatile size_t readTimes = 0;
  std::atomic<bool> stop{false};

  auto startTime = std::chrono::steady_clock::now();
  std::thread writeThread([&] {
    for (;;) {
      static size_t i;
      if (i++ == WriteNum) return;
      l.lockWrite([](const ST& l) {
        l->push_back(i);
        detail_printf("+: %zu, uc:%ld\n", l->size(), l.use_count());
      });
    }
  });
  std::thread deleteThread([&] {
    for (;;) {
      if (stop) return;
      l.lockWrite([&](const ST& l) {
        if (l->empty()) return;
        l->erase(l->cbegin());
        deleteNum++;
        detail_printf("-: %zu, uc:%ld\n", l->size(), l.use_count());
      });
    }
  });
  std::thread readThread([&] {
    for (;;) {
      l.lockRead([&](const ST& l) {
        (void)(l);
        readTimes++;
        detail_printf(" : %zu, uc:%ld\n", l->size(), l.use_count());
      });
      if (stop) return;
    }
  });

  writeThread.join();
  stop = true;
  deleteThread.join();
  readThread.join();
  auto endTime = std::chrono::steady_clock::now();

  remainNum = l->size();
  size_t timeUs = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
  printf("summary: +:%zu, -:%zu, =:%zu, r:%zu, timeUs:%zu\n", WriteNum, deleteNum, remainNum, readTimes, timeUs);
  if (WriteNum - deleteNum == remainNum) {
    printf("success!\n");
    return EXIT_SUCCESS;
  } else {
    return EXIT_FAILURE;
  }
}
