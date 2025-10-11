#ifdef _WIN32
#include <Windows.h>

#include <conio.h>
#else
#endif

#include "console_input.h"


#ifdef _WIN32
ConsoleInput::ConsoleInput() {}
ConsoleInput::~ConsoleInput() {}

std::optional<char> ConsoleInput::NonblockingGetChar() {
  if (_kbhit() == false) {
    return std::nullopt;
  }

  return static_cast<int32_t>(_getch());
}
#else
ConsoleInput::ConsoleInput() {}
ConsoleInput::~ConsoleInput() {}

std::optional<char> ConsoleInput::NonblockingGetChar() {
}
#endif