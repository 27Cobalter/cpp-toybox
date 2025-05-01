#include "argument_parser.h"

#include <algorithm>
#include <cassert>
#include <format>
#include <ranges>
#include <sstream>

#include <iostream>

inline std::string WrapValue(const ArgumentOption& arg, bool required);
inline std::string WrapBracket(std::string_view value, bool required);

ArgumentParser::ArgumentParser(std::string_view name, std::string_view version, std::string_view about,
                               std::optional<std::string_view> author, const std::vector<ArgumentOption>& arguments)
    : name_(name), version_(version), about_(about), author_(author) {
  for (const auto& arg : arguments) {
    if (arg.index.has_value()) {
      arg_index_.push_back(arg);
    } else {
      assert(arg.short_name.has_value() || arg.long_name.has_value());
      arg_options_.push_back(arg);
    }
  }
  arg_options_.emplace_back(
      ArgumentOption{.id = "help", .short_name = 'h', .long_name = "help", .help = "Show this help message and exit"});
  arg_options_.emplace_back(
      ArgumentOption{.id = "version", .short_name = 'v', .long_name = "version", .help = "Show this version and exit"});

  std::ranges::sort(arg_index_, {}, &ArgumentOption::index);

  bool required = true;
  bool hide     = false;
  for (auto i : std::views::iota(0u, arg_index_.size())) {

    assert(arg_index_[i].index.value() == (i + 1));
    assert(required == true || arg_index_[i].required == false);
    assert(hide == false || arg_index_[i].hide == true);

    required &= arg_index_[i].required;
    hide |= arg_index_[i].hide;
  }
}

Result ArgumentParser::Parse(int32_t argc, char** argv) {
  return Result::Help;
}

std::string ArgumentParser::Help() const {
  std::stringstream ss;

  ss << Version() << std::endl;
  if (author_.has_value()) {
    ss << std::format("Author: {}", author_.value()) << std::endl;
  }
  ss << about_ << std::endl;
  ss << std::endl;

  ss << std::format("Usage: {}{}", name_, arg_options_.empty() ? "" : " [OPTIONS]");
  for (const auto& index : arg_index_) {
    if (index.hide) continue;
    ss << std::format(" {}", WrapValue(index, index.required));
  }
  ss << std::endl << std::endl;

  std::string indent;
  indent = std::string(2, ' ');

  if (arg_index_.empty() == false) {
    ss << "Arguments:" << std::endl;
    std::stringstream ss_content;
    for (const auto& index : arg_index_) {
      if (index.hide) continue;
      ss_content << std::format("{}{}", indent, WrapValue(index, index.required)) << std::endl;
    }
    ss << AddAlignHelp(ss_content.str(), arg_index_);
  }

  if (arg_options_.empty() == false) {
    ss << std::endl;
    ss << "Options:" << std::endl;
    std::stringstream ss_content;
    for (const auto& arg : arg_options_) {
      if (arg.hide) continue;
      ss_content << indent;
      ss_content << std::format("{:2s}", arg.short_name.has_value() ? std::format("-{}", arg.short_name.value()) : "");
      if (arg.long_name.has_value()) {
        if (arg.short_name.has_value()) {
          ss_content << ", ";
        } else {
          ss_content << "  ";
        }
        ss_content << std::format("--{}", arg.long_name.value());
      }
      switch (arg.action) {
      case Action::Set:
      case Action::Append:
        ss_content << " " << WrapBracket(arg.value_name.value_or(arg.id), true);
        break;
      default:
        break;
      }
      ss_content << std::endl;
    }

    ss << AddAlignHelp(ss_content.str(), arg_options_);
  }

  return ss.str();
}

std::string ArgumentParser::AddAlignHelp(const std::string& content, const std::vector<ArgumentOption> args) const {
  std::stringstream separator;
  separator << std::endl;

  std::stringstream ss;

  auto show_count = std::ranges::count_if(args, [](const ArgumentOption& arg) { return arg.hide == false; });

  size_t max_length = 0;
  for (const auto& line : std::views::split(content, separator.str())) {
    max_length = std::max(line.size(), max_length);
  }

  auto arg_it = args.cbegin();
  for (const auto& line : std::views::split(content, separator.str()) | std::views::take(show_count)) {
    while (arg_it != args.cend() && arg_it->hide == true) {
      arg_it++;
    }
    ss << std::format("{:{}}  ", std::string{line.begin(), line.end()}, max_length);
    if (arg_it->help.has_value()) {
      ss << std::format("{} ", arg_it->help.value_or(""));
    }
    if (arg_it->default_value.has_value()) {
      ss << std::format("[default: {}] ", arg_it->default_value.value_or(""));
    }
    ss << std::endl;
    arg_it++;
  }

  return ss.str();
}

std::string ArgumentParser::Version() const {
  return std::format("{} {}", name_, version_);
}

inline std::string WrapValue(const ArgumentOption& arg, bool required) {
  return WrapBracket(arg.value_name.value_or(arg.id), required);
}

inline std::string WrapBracket(std::string_view value, bool required) {
  std::string_view prefix = required ? "<" : "[";
  std::string_view suffix = required ? ">" : "]";
  return std::format("{}{}{}", prefix, value, suffix);
}
