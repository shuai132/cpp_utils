#pragma once

#include <cstdint>
#include <memory>

namespace AlignedAlloc {

// Alignment must be power of 2 (1,2,4,8,16...)
inline void* aligned_malloc(size_t alignment, size_t size) {
  uintptr_t r = (uintptr_t)malloc(size + --alignment + sizeof(uintptr_t));
  uintptr_t t = r + sizeof(uintptr_t);
  uintptr_t o = (t + alignment) & ~(uintptr_t)alignment;
  if (!r) return nullptr;
  ((uintptr_t*)o)[-1] = r;
  return (void*)o;
}

inline void aligned_free(const void* p) {
  if (!p) return;
  free((void*)(((uintptr_t*)p)[-1]));
}

}  // namespace AlignedAlloc
