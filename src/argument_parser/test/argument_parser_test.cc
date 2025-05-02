#include <gtest/gtest.h>

#include <array>
#include <format>

#include "argument_parser.h"
#include "enum_utils.h"

void PrintTo(const Result& result, ::std::ostream* os) {
  *os << EnumName<Result>(result);
}

TEST(ArgumentParserTest, Help) {
  Command command("test", "Test Program", "0.0.1", std::nullopt,
                  {
                      ArgumentOption{.id = "REQUIRED1"},
                  });

  {
    std::array argv = {"test", "-h"};
    auto result     = command.TryParse(argv.size(), const_cast<char**>(argv.data()));
    ASSERT_EQ(result, Result::DisplayHelp);
  }
  {
    std::array argv = {"test", "--help"};
    auto result     = command.TryParse(argv.size(), const_cast<char**>(argv.data()));
    ASSERT_EQ(result, Result::DisplayHelp);
  }
}

TEST(ArgumentParserTest, Version) {
  Command command("test", "Test Program", "0.0.1", std::nullopt,
                  {
                      ArgumentOption{.id = "REQUIRED1"},
                  });

  {
    std::array argv = {"test", "-v"};
    auto result     = command.TryParse(argv.size(), const_cast<char**>(argv.data()));
    ASSERT_EQ(result, Result::DisplayVersion);
  }
  {
    std::array argv = {"test", "--version"};
    auto result     = command.TryParse(argv.size(), const_cast<char**>(argv.data()));
    ASSERT_EQ(result, Result::DisplayVersion);
  }
}

TEST(ArgumentParserTest, VersionNullOpt) {
  Command command("test", "Test Program", std::nullopt, std::nullopt,
                  {
                      ArgumentOption{.id = "REQUIRED1"},
                  });

  {
    std::array argv = {"test", "-v"};
    auto result     = command.TryParse(argv.size(), const_cast<char**>(argv.data()));
    ASSERT_EQ(result, Result::UnknownArgument);
  }
  {
    std::array argv = {"test", "--version"};
    auto result     = command.TryParse(argv.size(), const_cast<char**>(argv.data()));
    ASSERT_EQ(result, Result::UnknownArgument);
  }
}

TEST(ArgumentParserTest, Index) {
  std::array argv = {"test", "REQUIRED1", "REQUIRED2", "OPTIONAL1", "OPTIONAL2"};

  Command command("test", "Test Program", "0.0.1", std::nullopt,
                  {
                      ArgumentOption{.id = "REQUIRED1", .index = 1, .action = Action::Set, .required = true},
                      ArgumentOption{.id = "REQUIRED2", .index = 2, .action = Action::Set, .required = true},
                      ArgumentOption{.id = "OPTIONAL1", .index = 3, .action = Action::Set},
                      ArgumentOption{.id = "OPTIONAL2", .index = 4, .action = Action::Set},
                      ArgumentOption{.id = "OPTIONAL3", .index = 5, .action = Action::Set}
  });

  auto result = command.TryParse(argv.size(), const_cast<char**>(argv.data()));
  ASSERT_EQ(result, Result::Success);

  auto matches = command.Matches();
  ASSERT_FALSE(matches.expired());

  ASSERT_STREQ("REQUIRED1", matches.lock()->GetOne("REQUIRED1").value_or("").c_str());
  ASSERT_STREQ("REQUIRED2", matches.lock()->GetOne("REQUIRED2").value_or("").c_str());
  ASSERT_STREQ("OPTIONAL1", matches.lock()->GetOne("OPTIONAL1").value_or("").c_str());
  ASSERT_STREQ("OPTIONAL2", matches.lock()->GetOne("OPTIONAL2").value_or("").c_str());
  ASSERT_FALSE(matches.lock()->GetOne("OPTIONAL3").has_value());
}

TEST(ArgumentParserTest, IndexAppend) {
  std::array argv = {"test", "OPTIONAL1", "OPTIONAL2", "OPTIONAL3", "OPTIONAL4"};

  {
    Command command("test", "Test Program", "0.0.1", std::nullopt,
                    {
                        ArgumentOption{.id = "OPTIONAL1", .action = Action::Append},
    });

    auto result = command.TryParse(argv.size(), const_cast<char**>(argv.data()));
    ASSERT_EQ(result, Result::Success);

    auto matches = command.Matches();
    ASSERT_FALSE(matches.expired());

    ASSERT_EQ(4, matches.lock()->GetMany("OPTIONAL1").size());
  }
  {
    Command command("test", "Test Program", "0.0.1", std::nullopt,
                    {
                        ArgumentOption{.id = "OPTIONAL1", .index = 1, .action = Action::Set},
                        ArgumentOption{.id = "OPTIONAL2", .index = 2, .action = Action::Set},
                        ArgumentOption{.id = "OPTIONAL3", .action = Action::Append},
    });

    auto result = command.TryParse(argv.size(), const_cast<char**>(argv.data()));
    ASSERT_EQ(result, Result::Success);

    auto matches = command.Matches();
    ASSERT_FALSE(matches.expired());

    ASSERT_EQ(2, matches.lock()->GetMany("OPTIONAL3").size());
  }
}

TEST(ArgumentParserTest, Action) {
  std::array argv = {
      "test",      "-s=SETSHORT", "--setlong=SETLONG", "-a=APPEND1",       "-a",       "APPEND2",
      "-aAPPEND3", "-t",          "--settruelong",     "--append=APPEND4", "--append", "APPEND5",
  };

  Command command(
      "test", "Test Program", "0.0.1", std::nullopt,
      {
          ArgumentOption{.id = "setshort", .short_name = 's', .action = Action::Set},
          ArgumentOption{.id = "setlong", .long_name = "setlong", .action = Action::Set},
          ArgumentOption{.id = "append", .short_name = 'a', .long_name = "append", .action = Action::Append},
          ArgumentOption{.id = "settrueshort", .short_name = 't', .action = Action::SetTrue},
          ArgumentOption{.id = "settruelong", .long_name = "settruelong", .action = Action::SetTrue},
  });

  auto result = command.TryParse(argv.size(), const_cast<char**>(argv.data()));
  ASSERT_EQ(result, Result::Success);

  auto matches = command.Matches();
  ASSERT_FALSE(matches.expired());

  ASSERT_STREQ("SETSHORT", matches.lock()->GetOne("setshort").value_or("").c_str());
  ASSERT_STREQ("SETLONG", matches.lock()->GetOne("setlong").value_or("").c_str());
  auto values = matches.lock()->GetMany("append");
  ASSERT_EQ(5, values.size());
  ASSERT_STREQ("APPEND1", values[0].c_str());
  ASSERT_STREQ("APPEND2", values[1].c_str());
  ASSERT_STREQ("APPEND3", values[2].c_str());
  ASSERT_STREQ("APPEND4", values[3].c_str());
  ASSERT_STREQ("APPEND5", values[4].c_str());
  ASSERT_TRUE(matches.lock()->GetFlag("settrueshort").value_or(false));
  ASSERT_TRUE(matches.lock()->GetFlag("settruelong").value_or(false));
}

TEST(ArgumentParserTest, ActionSetTrueMultiple) {
  std::array argv = {"test", "-base", "-itvBASE"};

  Command command("test", "Test Program", "0.0.1", std::nullopt,
                  {
                      ArgumentOption{.id = "b", .short_name = 'b'},
                      ArgumentOption{.id = "a", .short_name = 'a'},
                      ArgumentOption{.id = "s", .short_name = 's'},
                      ArgumentOption{.id = "e", .short_name = 'e'},
                      ArgumentOption{.id = "i", .short_name = 'i'},
                      ArgumentOption{.id = "t", .short_name = 't'},
                      ArgumentOption{.id = "v", .short_name = 'v', .action = Action::Set},
  });

  auto result = command.TryParse(argv.size(), const_cast<char**>(argv.data()));
  ASSERT_EQ(result, Result::Success);

  auto matches = command.Matches();
  ASSERT_FALSE(matches.expired());

  ASSERT_TRUE(matches.lock()->GetFlag("b").value_or(false));
  ASSERT_TRUE(matches.lock()->GetFlag("a").value_or(false));
  ASSERT_TRUE(matches.lock()->GetFlag("s").value_or(false));
  ASSERT_TRUE(matches.lock()->GetFlag("e").value_or(false));
  ASSERT_TRUE(matches.lock()->GetFlag("i").value_or(false));
  ASSERT_TRUE(matches.lock()->GetFlag("t").value_or(false));
  ASSERT_STREQ("BASE", matches.lock()->GetOne("v").value_or("").c_str());
}
