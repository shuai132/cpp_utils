#include <thread>

#include "PerfTracker.h"

#define TRACK(msg)  perfTracker.track(std::string("L:") + std::to_string(__LINE__) + "\t"#msg)

int main() {
    PerfTracker<> perfTracker;
    // dumpHandle is optional
    perfTracker.dumpHandle = [](const std::string& result){
        std::cout << "PerfTracker result:\n" << result;
    };
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    TRACK("after 1");
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    TRACK("after 2");
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
    TRACK("after 3");
    return 0;
}
