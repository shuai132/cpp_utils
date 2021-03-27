#include <cstdio>
#include <vector>
#include <thread>
#include "copy_on_write.h"

int main() {
    copy_on_write<std::vector<int>> l;
    using SC = std::shared_ptr<std::vector<int>>;

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
            l.writeOp([](const SC& l) {
                l->push_back(i);
                printf("+: %zu, uc:%ld\n", l->size(), l.use_count());
            });
        }
    });
    std::thread deleteThread([&]{
        for(;;) {
            if (stop) return;
            l.writeOp([&](const SC& l) {
                if (l->empty()) return;
                l->erase(l->cbegin());
                deleteNum++;
                printf("-: %zu, uc:%ld\n", l->size(), l.use_count());
            });
        }
    });
    std::thread readThread([&]{
        for(;;) {
            l.readOp([&](const SC& l) {
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