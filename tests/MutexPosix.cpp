#include <cstdio>
#include <mutex>
#include <unistd.h>
#include "MutexPosix.h"

int main() {
    auto test = [](const char *name) {
        MutexPosix mutex("test");
        std::lock_guard<MutexPosix> lock(mutex);
        printf("%s: start: %d\n", name, getpid());
        sleep(1);
        printf("%s: end: %d\n", name, getpid());
    };

    auto pid = fork();
    if (pid == 0) {
        test("child");
    } else {
        test("parent");
        pause();
    }
    return 0;
}
