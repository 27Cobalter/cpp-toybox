#include "argument_parser.h"

#include <cstdint>
#include <iostream>

auto main(int32_t argc, char** argv) -> int32_t {
  for (int32_t i = 0; i < argc; ++i) {
    std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
  }
  return 0;
  Command command(
      "test", "Test program", "0.0.1", std::nullopt,
      {
          ArgumentOption{.id = "all", .short_name = 'a', .long_name = "all"},
          ArgumentOption{.id = "print", .short_name = 'p', .long_name = "print"},
          ArgumentOption{.id = "default", .short_name = 'd', .long_name = "default", .default_value = "default"},
          ArgumentOption{.id = "long", .long_name = "long"},
          ArgumentOption{.id = "short", .short_name = 's'},
          ArgumentOption{.id = "mlong", .long_name = "mlong", .action = Action::Set, .help = "Set argument"},
          ArgumentOption{.id = "nshort", .short_name = 'n', .action = Action::Append, .help = "Append argument"},
          ArgumentOption{.id            = "pos_req",
                         .index         = 1,
                         .required      = true,
                         .default_value = "default",
                         .help          = "Required positional argument"},
          ArgumentOption{.id = "pos_opt", .index = 2, .help = "Optional positional argument"},
          ArgumentOption{.id = "looooooooooooooong", .index = 3, .help = "long argument"},
  });

  auto result = command.TryParse(argc, argv);
  switch (result) {
  case Result::Success:
    break;
  case Result::DisplayVersion:
    std::cout << command.Version() << std::endl;
    return 0;
  case Result::DisplayHelp:
  default:
    std::cout << command.Help() << std::endl;
    return 0;
  }

  return 0;
}
