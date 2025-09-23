#include "ObserverCow.hpp"

#include <cstdio>
#include <string>

using namespace observer;

int main() {
  Dispose dispose;
  ObserverCow<std::string /*msg*/, int /*value*/> observer;
  observer
      ->connect([&](const std::string &msg, int value) {
        printf("msg: %s, value: %d\n", msg.c_str(), value);
        dispose.dispose();
      })
      .addTo(dispose);

  observer->emit("hello", 1);

  return 0;
}
