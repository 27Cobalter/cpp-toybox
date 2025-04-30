#include "Config.h"

#include <fstream>
#include <ranges>

#include <iostream>

namespace fs = std::filesystem;

Config::Config() {}

bool Config::Load(fs::path path) {
  if (fs::exists(path) == false) {
    return false;
  }
  std::ifstream ifs(path);
  for (const auto& line : std::views::istream<std::string>(ifs)) {
    constexpr std::string_view separator = "=";
    if (auto pos = line.find(separator); pos != std::string::npos) {
      map_.emplace(line.substr(0, pos), line.substr(pos + separator.size()));
    }
  }
  return true;
}

bool Config::Contains(const std::string& key) const {
  return map_.contains(key);
}

std::string Config::Get(const std::string& key) const {
  return map_.at(key);
}
