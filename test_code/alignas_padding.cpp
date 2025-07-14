#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

// x86 and most aarch64 is 64
// Apple M Series CPU is 128
#if defined(__APPLE__) && defined(__arm64__)
constexpr size_t CACHE_LINE_SIZE = 128;
#else
constexpr size_t CACHE_LINE_SIZE = 64;
#endif

struct alignas(CACHE_LINE_SIZE) CounterAligned1 {
  std::atomic<uint32_t> count{0};
};
// or
struct CounterAligned2 {
  alignas(CACHE_LINE_SIZE) std::atomic<uint32_t> count{0};
};
// or
struct CounterAligned3 {  // NOLINT(*-pro-type-member-init)
  std::atomic<uint32_t> count{0};
  uint8_t padding[CACHE_LINE_SIZE - sizeof(count)];
};

struct CounterNotAlign {
  std::atomic<uint32_t> count{0};
};

template <typename T>
static void increment_counter(T& counter, int iterations) {
  for (int i = 0; i < iterations; ++i) {
    ++counter.count;
  }
}

template <typename T>
static void test(const char* test_name) {
  int num_threads = 4;
  int iterations = 10000000;

  std::vector<T> counters(num_threads);

  std::vector<std::thread> threads;
  threads.reserve(num_threads);

  auto start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back(increment_counter<T>, std::ref(counters[i]), iterations);
  }

  for (auto& thread : threads) {
    thread.join();
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

  std::cout << "Test: " << test_name << std::endl;
  std::cout << "Number of Threads: " << num_threads << std::endl;
  std::cout << "Iterations per Thread: " << iterations << std::endl;
  std::cout << "Count: " << counters[0].count << std::endl;
  std::cout << "Time: " << duration.count() << " ms" << std::endl;
  std::cout << std::endl;
}

int main() {
  test<CounterAligned1>("CounterAligned1");
  test<CounterAligned2>("CounterAligned2");
  test<CounterAligned3>("CounterAligned3");
  test<CounterNotAlign>("CounterNotAlign");
  return 0;
}
