#include "argument_parser.h"

#include <cstdint>
#include <iostream>

auto main(int32_t argc, char** argv) -> int32_t {
  ArgumentParser parser(
      "test", "0.0.1", "Test program", std::nullopt,
      {
          ArgumentOption{.id = "all", .short_name = 'a', .long_name = "all"},
          ArgumentOption{.id = "print", .short_name = 'p', .long_name = "print"},
          ArgumentOption{.id = "default", .short_name = 'd', .long_name = "default", .default_value = "default"},
          ArgumentOption{.id = "long", .long_name = "long"},
          ArgumentOption{.id = "short", .short_name = 's'},
          ArgumentOption{.id = "mlong", .long_name = "mlong", .action = Action::Set, .help = "Set argument"},
          ArgumentOption{.id = "nshort", .short_name = 'n', .action = Action::Append, .help = "Append argument"},
          ArgumentOption{.id            = "pos_req",
                         .required      = true,
                         .index         = 1,
                         .default_value = "default",
                         .help          = "Required positional argument"},
          ArgumentOption{.id = "pos_opt", .index = 2, .help = "Optional positional argument"},
          ArgumentOption{.id = "looooooooooooooong", .index = 3, .help = "long argument"},
  });

  auto result = parser.Parse(argc, argv);
  switch (result) {
  case Result::Success:
    break;
  case Result::Version:
    std::cout << parser.Version() << std::endl;
    return 0;
  case Result::Help:
    [[fallthrough]];
  default:
    std::cout << parser.Help() << std::endl;
    return 0;
  }

  return 0;
}
