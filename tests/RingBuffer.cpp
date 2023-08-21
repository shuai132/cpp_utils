#include "RingBuffer.hpp"

#include <cstdio>
#include <memory>
#include <string>

struct Test {
  Test() {
    printf("Test: void\n");
  }

  Test(const char *id) : id(id) {  // NOLINT(google-explicit-constructor)
    printf("Test: normal constructor: %s\n", id);
  }

  Test(Test &&t) noexcept {
    printf("Test: move constructor: %s to %s\n", t.id.c_str(), this->id.c_str());
    this->id = t.id;
    t.id = "void";
  }

  Test &operator=(Test &&t) noexcept {
    printf("Test: move constructor: %s to %s\n", t.id.c_str(), this->id.c_str());
    this->id = t.id;
    t.id = "void";
    return *this;
  }

  ~Test() {
    printf("~Test: %s\n", id.c_str());
  }

  std::string id = "void";
  std::unique_ptr<int> unused;  // ensure unittest will not use copy constructor
};

int main() {
  RingBuffer<Test, 3> buffer;
  buffer.push_back("1");
  buffer.push_back("2");
  buffer.push_back("3");
  buffer.push_back("4");
  buffer.push_back("5");
  buffer.push_back("6");

  printf("==>\n");
  while (!buffer.empty()) {
    printf("%s \n", buffer.front().id.c_str());
    buffer.pop_front();
  }
  printf("<==\n");
  return 0;
}
