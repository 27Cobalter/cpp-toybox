#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

class Config {
private:
  std::unordered_map<std::string, std::string> map_;

public:
  Config();
  bool Load(std::filesystem::path path);
  bool Contains(const std::string& key) const;
  std::string Get(const std::string& key) const;
};
