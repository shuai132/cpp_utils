#include <stdexcept>

void func_with_except() {
  throw std::runtime_error("test");
}

void func_noexcept() noexcept {
  func_with_except();
}

int main() {
  try {
    func_noexcept();
  } catch (...) {
  }
  return 0;
}

// libc++abi: terminating due to uncaught exception of type std::runtime_error: test
