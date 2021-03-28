#include <vector>
#include <atomic>
#include <future>
#include "thread_safe.h"

int main() {
    const size_t kMaxCount = 10000;
    thread_safe<std::vector<int>> container;
    std::vector<std::future<void>> results;
    std::atomic<size_t> count{0};

    const int kThreadNum = 3;
    for(int i=0; i<kThreadNum; i++){
        auto t = std::async(std::launch::async, [&]{
            for(;;) {
                if (count++ >= kMaxCount) return;
                container->push_back(count);
                // or
                container.lock([&](const decltype(container)::ST& c){
                    c->push_back(count);
                });
            }
        });
        results.push_back(std::move(t));
    }

    for(auto& t : results){
        t.get();
    }
    printf("%zu\n", container->size());

    return 0;
}
