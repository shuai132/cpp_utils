#include <cstdio>
#include <vector>
#include <thread>
#include "thread_safe.h"

int main() {
    thread_safe<std::vector<int>, Type::CopyOnWrite> l;
    // or
    // copy_on_write<std::vector<int>> l;
    using ST = decltype(l)::ST;

    const size_t WriteNum = 10000;
    volatile size_t deleteNum = 0;
    volatile size_t remainNum = 0;
    volatile size_t readTimes = 0;
    std::atomic<bool> stop{false};

    auto startTime = std::chrono::steady_clock::now();
    std::thread writeThread([&]{
        for(;;) {
            static size_t i;
            if(i++ == WriteNum) return;
            l.lockWrite([](const ST& l) {
                l->push_back(i);
                printf("+: %zu, uc:%ld\n", l->size(), l.use_count());
            });
        }
    });
    std::thread deleteThread([&]{
        for(;;) {
            if (stop) return;
            l.lockWrite([&](const ST& l) {
                if (l->empty()) return;
                l->erase(l->cbegin());
                deleteNum++;
                printf("-: %zu, uc:%ld\n", l->size(), l.use_count());
            });
        }
    });
    std::thread readThread([&]{
        for(;;) {
            l.lockRead([&](const ST& l) {
                readTimes++;
                printf(" : %zu, uc:%ld\n", l->size(), l.use_count());
            });
            if (stop) return;
        }
    });

    writeThread.join();
    stop = true;
    deleteThread.join();
    readThread.join();
    auto endTime = std::chrono::steady_clock::now();

    remainNum = l->size();
    auto timeUs = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    printf("summary: +:%zu, -:%zu, =:%zu, r:%zu, timeUs:%llu\n", WriteNum, deleteNum, remainNum, readTimes, timeUs);
    if (WriteNum - deleteNum == remainNum) {
        printf("success!\n");
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}
