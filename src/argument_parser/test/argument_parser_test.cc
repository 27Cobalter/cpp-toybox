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
      "test",        "-s", "SETSHORT",      "--setlong", "SETLONG",    "-a",
      "APPENDSHORT", "-t", "--settruelong", "--append",  "APPENDLONG",
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
  ASSERT_EQ(2, values.size());
  ASSERT_STREQ("APPENDSHORT", values[0].c_str());
  ASSERT_STREQ("APPENDLONG", values[1].c_str());
  ASSERT_TRUE(matches.lock()->GetFlag("settrueshort").value_or(false));
  ASSERT_TRUE(matches.lock()->GetFlag("settruelong").value_or(false));
}

TEST(ArgumentParserTest, ActionSetAppendValueType) {
  std::array argv = {
      "test",
      "-a",
      "SETSHORTSPACE",
      "-b=SETSHORTEQUAL",
      "-cSETSHORTCONNECT",
      "-d",
      "APPENDSHORTSPACE",
      "-d=APPENDSHORTEQUAL",
      "-dAPPENDSHORTCONNECT",
      "--setlongspace",
      "SETLONGSPACE",
      "--setlongequal=SETLONGEQUAL",
      "--appendlong",
      "APPENDLONGSPACE",
      "--appendlong=APPENDLONGEQUAL",
  };

  Command command("test", "Test Program", "0.0.1", std::nullopt,
                  {
                      ArgumentOption{.id = "setshortspace",   .short_name = 'a',           .action = Action::Set   },
                      ArgumentOption{.id = "setshortequal",   .short_name = 'b',           .action = Action::Set   },
                      ArgumentOption{.id = "setshortconnect", .short_name = 'c',           .action = Action::Set   },
                      ArgumentOption{.id = "appendshort",     .short_name = 'd',           .action = Action::Append},
                      ArgumentOption{.id = "setlongspace",    .long_name = "setlongspace", .action = Action::Set   },
                      ArgumentOption{.id = "setlongequal",    .long_name = "setlongequal", .action = Action::Set   },
                      ArgumentOption{.id = "appendlong",      .long_name = "appendlong",   .action = Action::Append},
  });

  auto result = command.TryParse(argv.size(), const_cast<char**>(argv.data()));
  ASSERT_EQ(result, Result::Success);

  auto matches = command.Matches();
  ASSERT_FALSE(matches.expired());

  ASSERT_STREQ("SETSHORTSPACE", matches.lock()->GetOne("setshortspace").value_or("").c_str());
  ASSERT_STREQ("SETSHORTEQUAL", matches.lock()->GetOne("setshortequal").value_or("").c_str());
  ASSERT_STREQ("SETSHORTCONNECT", matches.lock()->GetOne("setshortconnect").value_or("").c_str());
  auto values_short = matches.lock()->GetMany("appendshort");
  ASSERT_EQ(3, values_short.size());
  ASSERT_STREQ("APPENDSHORTSPACE", values_short[0].c_str());
  ASSERT_STREQ("APPENDSHORTEQUAL", values_short[1].c_str());
  ASSERT_STREQ("APPENDSHORTCONNECT", values_short[2].c_str());
  ASSERT_STREQ("SETLONGSPACE", matches.lock()->GetOne("setlongspace").value_or("").c_str());
  ASSERT_STREQ("SETLONGEQUAL", matches.lock()->GetOne("setlongequal").value_or("").c_str());
  auto values_long = matches.lock()->GetMany("appendlong");
  ASSERT_EQ(2, values_long.size());
  ASSERT_STREQ("APPENDLONGSPACE", values_long[0].c_str());
  ASSERT_STREQ("APPENDLONGEQUAL", values_long[1].c_str());
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
