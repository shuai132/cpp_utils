#include <algorithm>

#include "Test.h"

template <typename T>
class my_vector {
 public:
  my_vector() = default;
  ~my_vector() {
    clear();
    free(data);
  }

  void push_back(T&& t) {
    if (size >= capacity) {
      capacity += 1;  // for test
      printf("capacity increase\n");
      auto new_data = reinterpret_cast<T*>(malloc(capacity * sizeof(T)));
      std::move((T*)data, (T*)data + size, new_data);
      free(data);
      data = new_data;
      auto new_addr = &((T*)data)[size++];
      new (new_addr) T(std::move(t));
    } else {
      auto new_addr = &((T*)data)[size++];
      new (new_addr) T(std::move(t));
    }
  }

  void clear() {
    for (size_t i = 0; i < size; ++i) {
      ((T*)data + i)->~T();
    }
  }

  T& operator[](size_t index) {
    return *(((T*)data) + index);
  }

 public:
  size_t capacity = 0;
  size_t size = 0;

  void* data = nullptr;
};

int main() {
  my_vector<Test> v;
  for (int i = 0; i < 2; ++i) {
    Test t(i);
    v.push_back(std::move(t));
  }

  printf("\n");
  for (int i = 0; i < v.size; i++) {
    printf("%d\n", v[i].v);
  }
  return 0;
}
