#include "RingBuffer.hpp"

#include <cstdio>
#include <memory>
#include <string>

#include "assert_def.h"
#include "log.h"

int count = 0;

struct Test {
  Test() {
    LOG("Test: void");
    ++count;
  }

  Test(const char *id) : id(id) {  // NOLINT(google-explicit-constructor)
    LOG("Test: normal constructor: %s", id);
    ++count;
  }

  Test(Test &&t) noexcept {
    LOG("Test: move constructor: %s to %s", t.id.c_str(), this->id.c_str());
    this->id = t.id;
    t.id = "void";
    ++count;
  }

  Test &operator=(Test &&t) noexcept {
    LOG("Test: move constructor: %s to %s", t.id.c_str(), this->id.c_str());
    this->id = t.id;
    t.id = "void";
    return *this;
  }

  ~Test() {
    LOG("~Test: %s", id.c_str());
    --count;
  }

  std::string id = "void";
  std::unique_ptr<int> unused;  // ensure unittest will not use copy constructor
};

int main() {
  {
    RingBuffer<Test, 3> buffer;
    ASSERT(count == 3);

    LOG("push: 1 start, c: %d", count);
    buffer.push_back("1");
    LOG("push: 1 end, : %d", count);
    ASSERT(buffer.size() == 1);
    ASSERT(count == 3);

    buffer.push_back("2");
    buffer.push_back("3");
    LOG("c: %d", count);
    ASSERT(count == 3);
    ASSERT(buffer.size() == 3);
    ASSERT(buffer.full());

    LOG("push: 4 start, c: %d", count);
    buffer.push_back("4");
    LOG("push: 4 end, c: %d", count);
    ASSERT(count == 3);
    ASSERT(buffer.size() == 3);
    ASSERT(buffer.full());

    buffer.push_back("5");
    buffer.push_back("6");
    buffer.push_back("7");
    buffer.push_back("8");
    LOG("push: 8 end, c: %d", count);
    ASSERT(count == 3);
    ASSERT(buffer.size() == 3);
    ASSERT(buffer.full());

    LOG("==>");
    auto it = buffer.begin();
    while (it != buffer.end()) {
      LOG("it: %s", (*it).id.c_str());
      it.next();
    }

    LOG("==>");
    while (!buffer.empty()) {
      LOG("%s ", buffer.front().id.c_str());
      buffer.pop_front();
    }
    LOG("<==");
    LOG("c: %d", count);

    ASSERT(buffer.empty());
    ASSERT(count == 3);
  }

  // destroy by RAII
  ASSERT(count == 0);

  return 0;
}
