#include <algorithm>

#include "Test.h"

template <typename T>
struct control_block {
  // noncopyable
  control_block(const control_block&) = delete;
  control_block() {
    printf("control_block: %p\n", this);
  };
  ~control_block() {
    printf("~control_block: %p\n", this);
    if (!from_make_shared) {
      // like default deleter
      delete p;
    }
  }
  T* p = nullptr;
  uint8_t unused{};
  std::atomic<size_t> ref_count{0};
  bool from_make_shared = false;
  std::atomic<size_t> weak_ref_count{0};
};

template <typename T>
class my_shared_ptr {
 public:
  my_shared_ptr() = default;
  explicit my_shared_ptr(T* t) {
    printf("my_shared_ptr: %p\n", this);
    cb = new control_block<T>();
    cb->p = t;
    ++cb->ref_count;
  }
  explicit my_shared_ptr(T* t, control_block<T>* cb) {
    printf("my_shared_ptr: make: %p\n", this);
    this->cb = cb;
    cb->p = t;
    ++(cb->ref_count);
    cb->from_make_shared = true;
  }
  my_shared_ptr& operator=(const my_shared_ptr<T>& s) {
    printf("my_shared_ptr& operator=\n");
    this->cb = s.cb;
    printf("my_shared_ptr& operator=: ref_count: %zu\n", cb->ref_count.load());
    ++cb->ref_count;
    printf("my_shared_ptr& operator=: ref_count: %zu\n", cb->ref_count.load());
    return *this;
  }
  ~my_shared_ptr() {
    --cb->ref_count;
    printf("~my_shared_ptr: ref_count: %zu\n", cb->ref_count.load());
    if (cb->ref_count == 0) {
      if (!cb->from_make_shared) {
        delete cb;
      } else {
        // mem: delete
        cb->p->~T();
        cb->~control_block();
        auto mem = (char*)cb->p;
        delete[] mem;
      }
    }
  }

  inline T* operator->() {
    return cb->p;
  }

 public:
  control_block<T>* cb = nullptr;
};

template <typename T, typename... Args>
my_shared_ptr<T> my_make_shared_simple(Args&&... args) {
  auto obj = new T(std::forward<Args>(args)...);
  auto ptr = my_shared_ptr<T>(obj);
  return ptr;
}

template <typename T, typename... Args>
my_shared_ptr<T> my_make_shared(Args&&... args) {
  // mem: new
  char* mem = new char[sizeof(T) + sizeof(control_block<T>)];
  auto obj = new (mem) T(std::forward<Args>(args)...);
  auto ctb = new (mem + sizeof(T)) control_block<T>();
  printf("obj: %p\n", obj);
  printf("ctb: %p\n", ctb);
  auto ptr = my_shared_ptr<T>(obj, ctb);
  return ptr;
}

int main() {
  my_shared_ptr<Test> sp;
  {
    my_shared_ptr<Test> sp1 = my_make_shared<Test>(1);
    printf("v: %d\n", sp1->v);
    sp = sp1;
  }
  printf("sp: v: %d\n", sp->v);
  return 0;
}
