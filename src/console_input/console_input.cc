#include <cassert>
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

std::optional<char> ConsoleInput::NonBlockingGetChar() {
  if (_kbhit() == true) {
    int32_t c = _getch();
    return c;
  } else {
    return std::nullopt;
  }
}

std::optional<Key> ConsoleInput::NonBlockingGetKey() {
  auto c = NonBlockingGetChar();

  if (c.has_value() == false) {
    return std::nullopt;
  }

  auto uc = static_cast<uint8_t>(c.value());

  if (uc >= '0' && uc <= '9') {
    return static_cast<Key>(static_cast<int32_t>(Key::Key_0) + (uc - '0')); // 0-9
  } else if (uc >= 'a' && uc <= 'z') {
    return static_cast<Key>(static_cast<int32_t>(Key::Key_A) + (uc - 'a')); // a-z
  } else if (uc >= 'A' && uc <= 'Z') {
    return static_cast<Key>(static_cast<int32_t>(Key::Key_A) + (uc - 'A')); // A-Z
  } else if (uc == 0) {
    auto uc2 = static_cast<uint8_t>(NonBlockingGetChar().value_or(0));
    if (uc2 >= 0x3b && uc2 <= 0x44) {
      return static_cast<Key>(static_cast<int32_t>(Key::Key_F1) + (uc2 - 0x3b)); // F1-F10
    } else {
      assert(false);
      return std::nullopt;
    }
  } else if (uc == 0xe0u) {
    auto uc2 = static_cast<uint8_t>(NonBlockingGetChar().value_or(0));
    switch (uc2) {
    case 0x85:
      return Key::Key_F11;
    case 0x86:
      return Key::Key_F12;
    case 0x48:
      return Key::Key_Up;
    case 0x4b:
      return Key::Key_Left;
    case 0x4d:
      return Key::Key_Right;
    case 0x50:
      return Key::Key_Down;
    default:
      std::println("c2: {:0x}", static_cast<int32_t>(uc2));
      assert(false);
      return std::nullopt;
    }
  } else {
    assert(false);
    return std::nullopt;
  }
}
void ConsoleInput::FlushInputBuffer() {
  while (_kbhit() == true) {
    auto _ = _getch();
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

std::optional<char> ConsoleInput::NonBlockingGetChar() {
  char c;
  if (read(STDIN_FILENO, &c, 1) == 1) {
    if (c == '\x1b') {
      std::println("");
      std::print("[0] esc ({:0x})", static_cast<int32_t>(c));
      char seq;
      for (int32_t i = 1; read(STDIN_FILENO, &seq, 1) == 1; i++) {
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
