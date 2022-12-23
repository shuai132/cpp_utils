#include "Shell.hpp"

#include <iostream>

int main() {
  auto result = Shell::exec("ls");
  std::cout << result << std::endl;
  return 0;
}
