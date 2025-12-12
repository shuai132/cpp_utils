#pragma once

#include <fstream>
#include <string>

#include "nlohmann/json.hpp"

class Settings {
 public:
  Settings() {
    try {
      json_ = nlohmann::json::parse(std::ifstream(file_path_));
    } catch (const std::exception& e) {
      // "json::parse error or no file found: " << e.what();
      json_ = nlohmann::json::parse("{}");
    }
  }
  ~Settings() {
    save();
  }

 public:
  void set_value(const std::string& key, const std::string& value) {
    json_[key] = value;
  };

  std::string get_value(const std::string& key) {
    auto v = json_[key];
    if (v.is_string()) {
      return v;
    } else {
      return "";
    }
  };

  void rm_value(const std::string& key) {
    json_.erase(key);
  };

  void clear() {
    std::ofstream ofs(file_path_);
    if (ofs) {
      ofs.write("{}", sizeof("{}") - 1);
      ofs.close();
    }
  }

  void save() {
    auto data = json_.dump();
    std::ofstream ofs(file_path_);
    if (ofs) {
      ofs.write(data.data(), (std::streamsize)data.size());
      ofs.close();
    }
  }

 private:
  const std::string file_path_ = "/tmp/xx.json";
  nlohmann::json json_;
};
