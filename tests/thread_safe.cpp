#include <vector>
#include <atomic>
#include <future>
#include "concurrent/thread_safe.h"

int main() {
    const size_t kMaxCount = 10000;

    thread_safe<std::vector<int>/*, Type::Default*/> container;
    {
        /// test constructor
        thread_safe<std::vector<int>> container2(1,2);
        thread_safe<std::vector<int>> container3{1,2,3};
    }
    container->size();
    container->push_back({});
    container->pop_back();

    thread_safe<std::vector<int>, Type::ReadWrite> container_rw;
    {
        /// test constructor
        thread_safe<std::vector<int>, Type::ReadWrite> container_rw2(1,2);
        thread_safe<std::vector<int>, Type::ReadWrite> container_rw3{1,2,3};
    }
    container_rw.read()->size();
    container_rw.write()->push_back({});
    container_rw.write()->pop_back();

    std::vector<std::future<void>> results;
    std::atomic<size_t> count{0};

    const int kThreadNum = 3;
    for(int i=0; i<kThreadNum; i++){
        auto t = std::async(std::launch::async, [&]{
            for(;;) {
                if (count++ >= kMaxCount) return;

                /// test container
                container->push_back({});
                // or
                container.lock([&](std::vector<int>* c){
                    c->push_back(count);
                });

                /// test container_rw
                container_rw.write()->push_back(count);
                // or
                container_rw.lockWrite([&](std::vector<int>* c){
                    c->push_back(count);
                });
            }
        });
        results.push_back(std::move(t));
    }

    for(auto& t : results){
        t.get();
    }
    printf("container:%s\n", container->size() == 2*kMaxCount ? "success" : "failed");
    printf("container_rw:%s\n", container_rw->size() == 2*kMaxCount ? "success" : "failed");

    return 0;
}
