#pragma once

#include <semaphore.h>

namespace sem_helper {

#if __linux__
inline bool set_value(sem_t *sem, int v) {
  int value = 0;
  int ret = sem_getvalue(sem, &value);
  if (ret == 0) {
    int diff = value - v;
    if (diff == 0) {
      return true;
    } else if (diff > 0) {
      while (value-- != v) {
        sem_trywait(sem);
      }
    } else {  // diff < 0
      while (value++ != v) {
        sem_post(sem);
      }
    }
    return true;
  } else {
    return false;
  }
}
#endif

}  // namespace sem_helper
