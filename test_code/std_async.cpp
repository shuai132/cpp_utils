#include <future>
#include <stdexcept>

void func_with_except() {
  throw std::runtime_error("test");
}

int main() {
  {
    auto f = std::async(std::launch::async, []() noexcept {
      func_with_except();
    });
    f.get();
  }

  return 0;
}

// libc++abi: terminating due to uncaught exception of type std::runtime_error: test
