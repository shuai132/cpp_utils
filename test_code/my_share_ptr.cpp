#include <atomic>
#include <mutex>
#include <new>

#include "AlignedAlloc.hpp"
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
  const char* mem = nullptr;
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
    cb->ref_count.fetch_add(1, std::memory_order_release);
  }

 private:
  explicit my_shared_ptr(const char* mem, T* t, control_block<T>* cb) {
    printf("my_shared_ptr: make: %p\n", this);
    this->cb = cb;
    cb->mem = mem;
    cb->p = t;
    cb->ref_count.fetch_add(1, std::memory_order_release);
    cb->from_make_shared = true;
  }
  template <typename T_, typename... Args>
  friend my_shared_ptr<T_> my_make_shared(Args&&... args);

 public:
  my_shared_ptr& operator=(const my_shared_ptr<T>& s) {
    printf("my_shared_ptr& operator=\n");
    this->cb = s.cb;
    auto count = cb->ref_count.fetch_add(1, std::memory_order_release);
    printf("my_shared_ptr& operator=: ref_count: %zu => %zu\n", count - 1, count);
    return *this;
  }
  ~my_shared_ptr() {
    auto count = cb->ref_count.fetch_sub(1, std::memory_order_acquire) - 1;
    printf("~my_shared_ptr: ref_count: %zu\n", count);
    if (count == 0) {
      if (!cb->from_make_shared) {
        delete cb;
      } else {
        // mem: delete
        cb->p->~T();
        cb->~control_block<T>();
        AlignedAlloc::aligned_free(cb->mem);
      }
    }
  }

  inline T* get() {
    return cb->p;
  }

  inline T* operator->() {
    return cb->p;
  }

  inline T& operator*() {
    return *(cb->p);
  }

 public:
  control_block<T>* cb = nullptr;
};

/// simple but will call new twice, for T and control_block
template <typename T, typename... Args>
my_shared_ptr<T> my_make_shared_simple(Args&&... args) {
  auto obj = new T(std::forward<Args>(args)...);
  auto ptr = my_shared_ptr<T>(obj);
  return ptr;
}

/// only call new once, but be care about type align
template <typename T, typename... Args>
my_shared_ptr<T> my_make_shared(Args&&... args) {
  constexpr size_t obj_size = sizeof(T);
  constexpr size_t obj_align = alignof(T);
  constexpr size_t cb_size = sizeof(control_block<T>);
  constexpr size_t cb_align = alignof(control_block<T>);

  size_t cb_offset = obj_size;
  if (cb_offset % cb_align != 0) {
    cb_offset += cb_align - (cb_offset % cb_align);
  }

  size_t total_size = cb_offset + sizeof(control_block<T>);
  printf("make_shared: total_size: %zu, obj_size: %zu, cb_size: %zu\n", total_size, obj_size, cb_size);
  printf("obj_align: %zu, cb_align: %zu\n", obj_align, cb_align);
  char* mem = (char*)AlignedAlloc::aligned_malloc(obj_align, total_size);
  printf("mem: %p\n", mem);

  auto obj = new (mem) T(std::forward<Args>(args)...);
  auto ctb = new (mem + cb_offset) control_block<T>();

  printf("obj: %p, obj_align: %zu, is_aligned: %d\n", obj, obj_align, (size_t)obj % obj_align == 0);
  printf("ctb: %p cb_align: %zu, offset: %zu, is_aligned: %d\n", ctb, cb_align, cb_offset, (size_t)ctb % cb_align == 0);

  return my_shared_ptr<T>(mem, obj, ctb);
}

struct alignas(64) TestAligned {
  int data;
};

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
    printf("\ntest:2\n");
    my_shared_ptr<Test> sp;
    {
      my_shared_ptr<Test> sp1 = my_make_shared<Test>(1);
      printf("v: %d\n", sp1->v);
      sp = sp1;
    }
    printf("sp: v: %d\n", sp->v);
  }
#endif

#if 1
  {
    printf("\ntest: std::atomic for test align\n");
    my_shared_ptr<std::atomic<size_t>> sp = my_make_shared<std::atomic<size_t>>();
    (*sp)++;
    bool is_aligned = (size_t)sp.get() % alignof(std::atomic<size_t>) == 0;
    printf("test: is_aligned: %d\n", is_aligned);
  }
#endif

#if 1
  {
    printf("\ntest: std::mutex for test align\n");
    my_shared_ptr<std::mutex> sp = my_make_shared<std::mutex>();
    sp->lock();
    sp->unlock();
    bool is_aligned = (size_t)sp.get() % alignof(std::mutex) == 0;
    printf("test: is_aligned: %d\n", is_aligned);
  }
#endif

#if 1
  {
    printf("\ntest: TestAligned for test align\n");
    my_shared_ptr<TestAligned> sp = my_make_shared<TestAligned>();
    bool is_aligned = (size_t)sp.get() % alignof(TestAligned) == 0;
    printf("test: is_aligned: %d\n", is_aligned);
  }
#endif
  return 0;
}
