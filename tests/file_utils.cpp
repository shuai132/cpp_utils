#include "file_utils.hpp"

int main(int argc, char** argv) {
  (void)(argc);
  size_t size;
  auto data = file_utils::read_file(argv[0], &size);
  if (!data) {
    printf("read failed: %s\n", argv[0]);
  } else {
    printf("write file_utils.out: %zu\n", size);
    file_utils::write_to_file("file_utils.out", data.get(), size);
  }
  return 0;
}
