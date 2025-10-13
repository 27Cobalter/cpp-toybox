#include <print>

#ifdef _WIN32
#include <Windows.h>

#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#include "console_input.h"

#ifdef _WIN32
ConsoleInput::ConsoleInput() {}
ConsoleInput::~ConsoleInput() {}

std::optional<char> ConsoleInput::NonblockingGetChar() {
  if (_kbhit() == true) {
    int32_t c = _getch();
    if (c == 0 || c == 0xe0) {
      std::println("");
      std::print("[0] special ({:0x})", static_cast<int32_t>(c));
      for (int32_t i = 0; _kbhit() == true; i++) {
        int32_t c2;
        c2 = _getch();
        std::print(", [{}] {} ({:0x})", i, static_cast<char>(c2), static_cast<int32_t>(c2));
      }
      std::println("");
      return std::nullopt;
    }
    return static_cast<char>(c);
  } else {
    return std::nullopt;
  }
}
#else
ConsoleInput::ConsoleInput() {
  termios* old = new termios();
  context_     = reinterpret_cast<void*>(old);

  tcgetattr(STDIN_FILENO, old);
  termios tnew = *old;
  tnew.c_lflag &= ~(ICANON | ECHO);
  tnew.c_cc[VMIN]  = 0;
  tnew.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSANOW, &tnew);
}
ConsoleInput::~ConsoleInput() {
  termios* old = reinterpret_cast<termios*>(context_);
  tcsetattr(STDIN_FILENO, TCSANOW, old);
  delete old;
}

std::optional<char> ConsoleInput::NonblockingGetChar() {
  char c;
  if (read(STDIN_FILENO, &c, 1) == 1) {
    if (c == '\x1b') {
      std::println("");
      std::print("[0] esc ({:0x})", static_cast<int32_t>(c));
      char seq;
      for (int32_t i = 0; read(STDIN_FILENO, &seq, 1) == 1; i++) {
        std::print(", [{}] {} ({:0x})", i, seq, static_cast<int32_t>(seq));
      }
      std::println("");
      return std::nullopt;
    }
    return c;
  }
  return std::nullopt;
}
#endif
