#include <cstdio>
#include <vector>
#include "copy_on_write.h"

int main() {
    copy_on_write<std::vector<int>> l;
    l.writeOp([](const std::shared_ptr<std::vector<int>>& l) {
        l->push_back(1);
        printf("%d, use_count:%ld\n", (*l)[0], l.use_count());
    });
    l.readOp([](const std::shared_ptr<std::vector<int>>& l) {
        printf("%d, use_count:%ld\n", (*l)[0], l.use_count());
    });
    return 0;
}
