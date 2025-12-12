#pragma once

#include <type_traits>

#define FOR(i, n) for (std::remove_reference<std::remove_const<decltype(n)>::type>::type i = 0; i < n; i++)

template <typename Container, typename Func>
inline void foreach (Container&& c, Func && f) {
  for (auto& v : c) {
    f(v);
  }
}
