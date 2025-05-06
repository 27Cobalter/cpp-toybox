#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <span>
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
  std::optional<char> short_name                = std::nullopt;
  std::optional<std::string_view> long_name     = std::nullopt;
  std::optional<int32_t> index                  = std::nullopt;
  Action action                                 = Action::Set;
  bool required                                 = false;
  bool hide                                     = false;
  std::optional<std::string_view> value_name    = std::nullopt;
  std::optional<std::string_view> default_value = std::nullopt;
  /// std::optional<std::string_view> requires_all       = std::nullopt;
  // std::optional<std::string_view> conflicts_with_all = std::nullopt;
  // std::optional<std::string_view> value_delimiter    = std::nullopt;
  std::optional<std::string_view> help = std::nullopt;
};

enum class Result {
  Success,
  UnknownArgument,
  TooManyValues,
  TooFewValues,
  ArgumentConflict,
  MissingRequiredArgument,
  MissingSubCommand,
  DisplayHelp,
  DisplayVersion,

  UnknownError,
  NotImplemented,
};

class ArgMatches {
public:
  std::optional<std::string> GetOne(std::string_view id) const;
  std::vector<std::string> GetMany(std::string_view id) const;
  bool GetFlag(std::string_view id) const;

  void Set(std::string_view id, std::string_view);
  void Append(std::string_view id, std::string_view);
  void SetTrue(std::string_view id);

  bool HasOne(std::string_view id) const;
  bool HasMany(std::string_view id) const;
  bool HasFlag(std::string_view id) const;

  void SetSubCommand(std::string_view name, std::shared_ptr<ArgMatches> matches);
  std::optional<std::pair<std::string_view, std::weak_ptr<const ArgMatches>>> SubCommand() const;
  std::optional<std::string_view> SubCommandName() const;
  std::weak_ptr<const ArgMatches> SubCommandMatches(std::string_view name) const;

private:
  std::unordered_map<std::string_view, std::string> set_value_;
  std::unordered_map<std::string_view, std::vector<std::string>> append_value_;
  std::unordered_map<std::string_view, bool> set_true_value_;
  std::optional<std::pair<std::string_view, std::shared_ptr<ArgMatches>>> sub_matches_;
};

class SubCommand;

class Command {
public:
  Command(std::string_view name, std::string_view about, std::optional<std::string_view> version,
          std::optional<std::string_view> author, const std::vector<ArgumentOption>& arguments,
          const std::vector<Command>& subcommands = {}, bool subcommand_required = false);
  Command(std::string_view name, std::string_view about, const std::vector<ArgumentOption>& arguments,
          const std::vector<Command>& subcommands = {}, bool subcommand_required = false);
  ~Command() = default;
  Result TryParse(int32_t argc, char** argv);
  std::string Help() const;
  std::string Version() const;

  std::weak_ptr<const ArgMatches> Matches() const;

private:
  std::string Help_Impl(std::string_view parent_text) const;
  std::string AddAlignAbout(const std::string& str) const;
  std::string AddAlignHelp(const std::string& str, const std::vector<ArgumentOption>& args) const;

  using arg_it_t = std::span<char*>::iterator;
  std::tuple<arg_it_t, Result> ParseOptionLong(const arg_it_t& it, const arg_it_t& end, std::string_view content);
  std::tuple<arg_it_t, Result> ParseOptionShort(const arg_it_t& it, const arg_it_t& end, std::string_view content);
  std::tuple<arg_it_t, Result> ParseArgument(const arg_it_t& it, const arg_it_t& end, int32_t& index,
                                             std::string_view content);

  bool HasAuto(const ArgumentOption& arg) const;
  void SetAuto(const ArgumentOption& arg, std::string_view content);

private:
  const std::string_view name_;
  const std::optional<std::string_view> version_;
  const std::string_view about_;
  const std::optional<std::string_view> author_;
  const bool subcommand_required_;
  ArgumentOption arg_help_;
  ArgumentOption arg_version_;
  std::vector<ArgumentOption> arg_index_;
  std::vector<ArgumentOption> arg_options_;
  std::unordered_map<std::string_view, Command> subcommands_;
  std::shared_ptr<ArgMatches> arg_result_;
};
