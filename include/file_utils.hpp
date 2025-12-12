#pragma once

#include <cstdint>
#include <fstream>
#include <memory>

namespace file_utils {

inline std::shared_ptr<uint8_t> read_file(const std::string &file, size_t *size) {
  std::ifstream ifs(file, std::ios::binary);
  if (!ifs) {
    return nullptr;
  }
  ifs.seekg(0, std::ios::end);
  std::streampos file_size = ifs.tellg();
  if (file_size < 0) {
    return nullptr;
  }
  *size = static_cast<size_t>(file_size);
  ifs.seekg(0, std::ios::beg);

  std::shared_ptr<uint8_t> file_data(new uint8_t[*size], [](const uint8_t *p) {
    delete[] p;
  });
  char *ptr = reinterpret_cast<char *>(file_data.get());

  ifs.read(ptr, static_cast<std::streamsize>(*size));
  if (!ifs) {
    return nullptr;
  }
  ifs.close();
  return file_data;
}

inline bool write_to_file(const std::string &file_name, const void *data, size_t size_in_bytes) {
  std::ofstream outfile(file_name, std::ofstream::binary);
  if (!outfile) {
    return false;
  }
  outfile.write((char *)data, size_in_bytes);  // NOLINT(cppcoreguidelines-narrowing-conversions)
  outfile.close();
  return true;
}

}  // namespace file_utils
