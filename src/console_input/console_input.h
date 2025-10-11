#pragma once

#include <optional>

class ConsoleInput {
public:
  ConsoleInput();
  ~ConsoleInput();
  std::optional<char> NonblockingGetChar();
};