#include "Reference.hpp"

struct Test {
  Test() {
    printf("new...\n");
  }

  ~Test() {
    printf("delete...\n");
  }

  void print() const {
    printf("v: %d\n", v);
  }

  int v = 1;
};

int main() {
  Reference<Test> ref;
  ref.retain();
  ref.retain();
  ref->print();
  ref.release();
  ref.release();
  return 0;
}
