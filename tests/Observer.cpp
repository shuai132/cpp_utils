#include "Observer.hpp"

using namespace observer;

struct MainThreadScheduler : public Scheduler {
  void call(Callable callable) override {
    // schedule callable to main thread here
    // this demo just run it directly
    callable();
  }
};
static auto MainThread = std::make_shared<MainThreadScheduler>();  // NOLINT

int main() {
  /// 1. define observer, can be any type
  Observer<std::string /*msg*/, int /*value*/> observer;

  {
    /// `Dispose` can auto disconnect observer by RAII
    Dispose dispose;

    /// 2.1 connect
    observer
        // connect, support call more than once
        // will return `Disposable` for disconnect by hand, call .dispose()
        ->connect([](const std::string &msg, int value) {
          printf("msg: %s, value: %d\n", msg.c_str(), value);
        })
        // `Disposable` can be added to dispose, will auto disconnect it on dispose destroy.
        .addTo(dispose);

    /// 2.2 connect and disconnect
    observer
        ->connect([](const std::string &msg, int value) {
          printf("will not be called\n");
        })
        // disconnect
        .dispose();

    /// 2.3 connect with scheduler
    observer
        ->connect(
            [](const std::string &msg, int value) {
              printf("will run on main thread\n");
            },
            MainThread)
        .addTo(dispose);

    /// 3. emit
    observer->emit("hello", 1);
  }

  /// 4. dispose destroyed now, so emit will do nothing
  printf("will print nothing after here\n");
  observer->emit("nothing", 2);

  return 0;
}
