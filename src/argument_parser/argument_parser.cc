#include "argument_parser.h"

#include <algorithm>
#include <cassert>
#include <format>
#include <limits>
#include <ranges>
#include <sstream>

#include <iostream>

inline std::string WrapValue(const ArgumentOption& arg, bool required);
inline std::string WrapBracket(std::string_view value, bool required);

std::optional<std::string> ArgMatches::GetOne(std::string_view id) const {
  auto pos = set_value_.find(id);
  if (pos == set_value_.end()) {
    return std::nullopt;
  } else {
    return pos->second;
  }
}
std::vector<std::string> ArgMatches::GetMany(std::string_view id) const {
  auto pos = append_value_.find(id);
  if (pos == append_value_.end()) {
    return std::vector<std::string>{};
  } else {
    return pos->second;
  }
}
std::optional<bool> ArgMatches::GetFlag(std::string_view id) const {
  auto pos = set_true_value_.find(id);
  if (pos == set_true_value_.end()) {
    return std::nullopt;
  } else {
    return pos->second;
  }
}

void ArgMatches::Set(std::string_view id, std::string_view value) {
  set_value_.insert_or_assign(id, value);
}
void ArgMatches::Append(std::string_view id, std::string_view value) {
  if (append_value_.contains(id) == false) {
    append_value_.emplace(id, std::vector<std::string>{});
  }
  append_value_[id].emplace_back(value);
}
void ArgMatches::SetTrue(std::string_view id) {
  set_true_value_.insert_or_assign(id, true);
}

bool ArgMatches::HasOne(std::string_view id) const {
  return set_value_.contains(id);
}
bool ArgMatches::HasMany(std::string_view id) const {
  return append_value_.contains(id);
}
bool ArgMatches::HasFlag(std::string_view id) const {
  return set_true_value_.contains(id);
}

Command::Command(std::string_view name, std::string_view about, std::optional<std::string_view> version,
                 std::optional<std::string_view> author, const std::vector<ArgumentOption>& arguments,
                 const std::vector<Command>& sub_commands, bool sub_command_required)
    : name_(name), version_(version), about_(about), author_(author), arg_result_(nullptr) {
  for (const auto& arg : arguments) {
    if (arg.short_name.has_value() || arg.long_name.has_value()) {
      arg_options_.push_back(arg);
    } else {
      arg_index_.push_back(arg);
    }
  }
  arg_help_ =
      ArgumentOption{.id = "help", .short_name = 'h', .long_name = "help", .help = "Show this help message and exit"};
  arg_version_ =
      ArgumentOption{.id = "version", .short_name = 'v', .long_name = "version", .help = "Show this version and exit"};

  std::ranges::sort(arg_index_, {},
                    [](const auto& arg) { return arg.index.value_or(std::numeric_limits<int32_t>::max()); });

  bool required = true;
  bool hide     = false;
  for (auto i : std::views::iota(0u, arg_index_.size())) {
    assert(i == (arg_index_.size() - 1) || arg_index_[i].index.value() == (i + 1));
    assert(i == (arg_index_.size() - 1) || arg_index_[i].action == Action::Set);
    assert(required == true || arg_index_[i].required == false);
    assert(hide == false || arg_index_[i].hide == true);

    required &= arg_index_[i].required;
    hide |= arg_index_[i].hide;
  }
}

Result Command::TryParse(int32_t argc, char** argv) {
  arg_result_ = std::make_shared<ArgMatches>();

  std::span<char*> args{argv, static_cast<size_t>(argc)};

  Result result = Result::Success;
  int32_t index = 0;
  for (auto it = args.cbegin() + 1; it != args.cend();) {
    std::string_view arg{*it};

    if (arg.length() >= 2 && arg.starts_with("--")) {
      std::tie(it, result) = ParseOptionLong(it, args.cend(), arg.substr(2));
    } else if (arg.starts_with("-")) {
      std::tie(it, result) = ParseOptionShort(it, args.cend(), arg.substr(1));
    } else {
      std::tie(it, result) = ParseArgument(it, args.cend(), index, arg);
    }
  }
  return result;
}

using arg_it_t = std::span<char*>::const_iterator;
// Support --arg=value and --arg value
std::tuple<arg_it_t, Result> Command::ParseOptionLong(const arg_it_t& it, const arg_it_t& end,
                                                      std::string_view content) {
  if (content.empty() == true) {
    return {end, Result::NotImplemented};
  }

  if (content == "help") {
    return {end, Result::DisplayHelp};
  }
  if (version_.has_value() && content == "version") {
    return {end, Result::DisplayVersion};
  }

  for (const auto& arg : arg_options_) {
    if (arg.long_name.has_value() == false) {
      continue;
    }
    if (arg.long_name.value() == content) {
      switch (arg.action) {
      case (Action::Set):
        if (it + 1 == end) {
          return {end, Result::TooFewValues};
        }
        arg_result_->Set(arg.id, *(it + 1));
        return {it + 2, Result::Success};
      case (Action::Append):
        if (it + 1 == end) {
          return {end, Result::TooFewValues};
        }
        arg_result_->Append(arg.id, *(it + 1));
        return {it + 2, Result::Success};
      case (Action::SetTrue):
        arg_result_->SetTrue(arg.id);
        return {it + 1, Result::Success};
      default:
        assert(false);
        break;
      }
    } else if (content.starts_with(std::format("{}=", arg.long_name.value()))) {
      switch (arg.action) {
      case (Action::Set): {
        auto pos = content.find('=');
        arg_result_->Set(arg.id, content.substr(pos + 1));
        return {it + 1, Result::Success};
      }
      case (Action::Append): {
        auto pos = content.find('=');
        arg_result_->Append(arg.id, content.substr(pos + 1));
        return {it + 1, Result::Success};
      }
      case (Action::SetTrue):
        return {end, Result::UnknownArgument};
      default:
        assert(false);
        break;
      }
    }
  }
  return {end, Result::UnknownArgument};
}

// Support -a=value and -a value and -avalue
std::tuple<arg_it_t, Result> Command::ParseOptionShort(const arg_it_t& it, const arg_it_t& end,
                                                       std::string_view content) {
  if (content.empty() == true) {
    return {end, Result::NotImplemented};
  }

  if (content == "h") {
    return {end, Result::DisplayHelp};
  }
  if (version_.has_value() && content == "v") {
    return {end, Result::DisplayVersion};
  }

  for (auto c_it = content.begin(); c_it != content.end(); c_it++) {
    bool found = false;
    for (const auto& arg : arg_options_) {
      if (arg.short_name.has_value() == false) {
        continue;
      }
      if (arg.short_name.value() == *c_it) {
        switch (arg.action) {
        case (Action::Set): {
          if (c_it + 1 != content.end()) {
            std::string_view value;
            if (*(c_it + 1) == '=') {
              value = std::string_view{c_it + 2, content.end()};
            } else {
              value = std::string_view{c_it + 1, content.end()};
            }
            arg_result_->Set(arg.id, value);
            return {it + 1, Result::Success};
          } else {
            if (it + 1 == end) {
              return {end, Result::TooFewValues};
            }
            arg_result_->Set(arg.id, *(it + 1));
            return {it + 2, Result::Success};
          }
        }
        case (Action::Append): {
          if (c_it + 1 != content.end()) {
            std::string_view value;
            if (*(c_it + 1) == '=') {
              value = std::string_view{c_it + 2, content.end()};
            } else {
              value = std::string_view{c_it + 1, content.end()};
            }
            arg_result_->Append(arg.id, value);
            return {it + 1, Result::Success};
          } else {
            if (it + 1 == end) {
              return {end, Result::TooFewValues};
            }
            arg_result_->Append(arg.id, *(it + 1));
            return {it + 2, Result::Success};
          }
        }
        case (Action::SetTrue):
          arg_result_->SetTrue(arg.id);
          break;
        }
      }
      found = true;
    }
    if (found == false) {
      return {end, Result::UnknownArgument};
    }
  }
  return {it + 1, Result::Success};
}

std::tuple<arg_it_t, Result> Command::ParseArgument(const arg_it_t& it, const arg_it_t& end, int32_t& index,
                                                    std::string_view content) {
  if (index >= arg_index_.size()) {
    return {end, Result::TooManyValues};
  }
  auto& arg = arg_index_[index];
  switch (arg.action) {
  case Action::Set:
    arg_result_->Set(arg.id, content);
    index++;
    break;
  case Action::Append:
    arg_result_->Append(arg.id, content);
    break;
  case Action::SetTrue:
    arg_result_->SetTrue(arg.id);
    index++;
    break;
  default:
    assert(false);
    break;
  }
  return {it + 1, Result::Success};
}

std::weak_ptr<const ArgMatches> Command::Matches() const {
  return arg_result_;
}

std::weak_ptr<const ArgMatches> Command::SubCommandMatches(std::string_view name) const {
  // auto it = std::ranges::find_if(sub_commands_, [&name](const Command& cmd) { return cmd.name_ == name; });
  // if (it != sub_commands_.end()) {
  //   return it->Matches();
  // }
  // return nullptr;
  return {};
}

std::string Command::Help() const {
  std::stringstream ss;
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

    auto AppendContent = [&ss_content, &indent](const ArgumentOption& arg) {
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
    };
    for (const auto& arg : arg_options_) {
      if (arg.hide) continue;
      AppendContent(arg);
    }
    AppendContent(arg_help_);
    if (version_.has_value()) {
      AppendContent(arg_version_);
    }

    ss << AddAlignHelp(ss_content.str(), arg_options_);
  }

  return ss.str();
}

std::string Command::AddAlignHelp(const std::string& content, const std::vector<ArgumentOption>& args) const {
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

std::string Command::Version() const {
  assert(version_.has_value() == true);
  return std::format("{} {}", name_, version_.value());
}

inline std::string WrapValue(const ArgumentOption& arg, bool required) {
  return WrapBracket(arg.value_name.value_or(arg.id), required);
}

inline std::string WrapBracket(std::string_view value, bool required) {
  std::string_view prefix = required ? "<" : "[";
  std::string_view suffix = required ? ">" : "]";
  return std::format("{}{}{}", prefix, value, suffix);
}
