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
  std::atomic<size_t> ref_count{0};
  std::atomic<size_t> weak_ref_count{0};
  bool from_make_shared = false;
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
        cb->~control_block<T>();
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
  // 计算对齐后的偏移量
  constexpr size_t obj_size = sizeof(T);
  constexpr size_t obj_align = alignof(T);
  constexpr size_t cb_align = alignof(control_block<T>);

  // 计算 control_block 的对齐偏移
  size_t cb_offset = obj_size;
  if (cb_offset % cb_align != 0) {
    cb_offset += cb_align - (cb_offset % cb_align);
  }

  // 分配足够的内存
  size_t total_size = cb_offset + sizeof(control_block<T>);
  char* mem = new char[total_size];

  // 在对齐的位置构造对象
  auto obj = new (mem) T(std::forward<Args>(args)...);
  auto ctb = new (mem + cb_offset) control_block<T>();

  printf("obj: %p (align: %zu)\n", obj, obj_align);
  printf("ctb: %p (align: %zu, offset: %zu)\n", ctb, cb_align, cb_offset);

  auto ptr = my_shared_ptr<T>(obj, ctb);
  return ptr;
}

int main() {
#if 1
  {
    printf("test:1 simple\n");
    my_shared_ptr<Test> sp = my_shared_ptr<Test>(new Test());
    printf("test:1 v: %d\n", sp->v);
  }
#endif

#if 1
  {
    printf("test:2\n");
    my_shared_ptr<Test> sp;
    {
      my_shared_ptr<Test> sp1 = my_make_shared<Test>(1);
      printf("v: %d\n", sp1->v);
      sp = sp1;
    }
    printf("sp: v: %d\n", sp->v);
  }
#endif
  return 0;
}
