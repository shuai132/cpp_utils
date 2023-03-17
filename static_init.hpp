#pragma once

#include <functional>

class StaticCall {
 public:
  explicit StaticCall(const std::function<void()>& func) noexcept {
    func();
  }
};

// clang-format off

#define __STATIC_CALL_CAT_(a, b)    a##b
#define __STATIC_CALL_CAT(a, b)     __STATIC_CALL_CAT_(a, b)
#define static_init(func)           namespace { \
                                    StaticCall __STATIC_CALL_CAT(__static_call__var, __COUNTER__)(func);  /* NOLINT(cert-err58-cpp) */ \
                                    }
