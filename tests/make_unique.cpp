#include "make_unique.h"

int main() {
    auto value = std::make_unique<int>(123);
    printf("%d", *value);
    return 0;
}
