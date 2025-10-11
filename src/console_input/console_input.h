#pragma once

#include <optional>

class ConsoleInput {
private:
  void* context_;
public:
  ConsoleInput();
  ~ConsoleInput();
  std::optional<char> NonblockingGetChar();
};
