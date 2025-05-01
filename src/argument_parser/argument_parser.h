#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

enum class Action {
  Set,
  Append,
  SetTrue,
};

struct ArgumentOption {
  std::string_view id;
  std::optional<char> short_name                     = std::nullopt;
  std::optional<std::string_view> long_name          = std::nullopt;
  bool required                                      = false;
  bool hide                                          = false;
  Action action                                      = Action::SetTrue;
  std::optional<int32_t> index                       = std::nullopt;
  std::optional<std::string_view> value_name         = std::nullopt;
  std::optional<std::string_view> default_value      = std::nullopt;
  std::optional<std::string_view> requires_all       = std::nullopt;
  std::optional<std::string_view> conflicts_with_all = std::nullopt;
  std::optional<std::string_view> value_delimiter    = std::nullopt;
  std::optional<std::string_view> help               = std::nullopt;
};

enum class Result {
  Success,
  Help,
  Version,
};

class ArgumentParser {
public:
  ArgumentParser(std::string_view name, std::string_view version, std::string_view about,
                 std::optional<std::string_view> author, const std::vector<ArgumentOption>& arguments);
  ~ArgumentParser() = default;
  Result Parse(int32_t argc, char** argv);
  std::string Help() const;
  std::string Version() const;

private:
  std::string AddAlignHelp(const std::string& str, const std::vector<ArgumentOption> args) const;

private:
  const std::string_view name_;
  const std::string_view version_;
  const std::string_view about_;
  const std::optional<std::string_view> author_;
  std::vector<ArgumentOption> arg_index_;
  std::vector<ArgumentOption> arg_options_;
  std::unordered_map<std::string_view, std::string_view> set_value_;
  std::unordered_map<std::string_view, std::vector<std::string_view>> append_value_;
  std::unordered_map<std::string_view, bool> set_true_value_;
};
