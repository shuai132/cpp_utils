#include "Shell.hpp"

#include <cstdio>
#include <iostream>
#include <thread>

int main() {
  auto result = Shell::exec("uname -a");
  std::cout << result << std::endl;

  std::thread([&] {
    Shell::exec("sleep 1");
    printf("t1 end\n");
  }).detach();

  std::thread([&] {
    Shell::exec("sleep 3");
    printf("t2 end\n");
  }).join();

  return 0;
}
