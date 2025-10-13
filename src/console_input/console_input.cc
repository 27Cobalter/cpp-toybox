#ifdef _WIN32
#include <Windows.h>
#endif

#include <cassert>
#include <cstdint>

#include "console_input.h"

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#ifdef _WIN32
ConsoleInput::ConsoleInput() {}
ConsoleInput::~ConsoleInput() {}

std::optional<char> ConsoleInput::TryGetChar() {
  if (_kbhit() == true) {
    int32_t c = _getch();
    return c;
  } else {
    return std::nullopt;
  }
}

std::optional<Key> ConsoleInput::TryGetKey() {
  auto c = TryGetChar();

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
    auto uc2 = static_cast<uint8_t>(TryGetChar().value_or(0));
    if (uc2 >= 0x3b && uc2 <= 0x44) {
      return static_cast<Key>(static_cast<int32_t>(Key::Key_F1) + (uc2 - 0x3b)); // F1-F10
    } else {
      assert(false);
    }
  } else if (uc == 0xe0u) {
    auto uc2 = static_cast<uint8_t>(TryGetChar().value_or(0));
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
      assert(false);
    }
  }

  assert(false);
  return std::nullopt;
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

std::optional<char> ConsoleInput::TryGetChar() {
  char c;
  if (read(STDIN_FILENO, &c, 1) == 1) {
    return c;
  }
  return std::nullopt;
}

std::optional<Key> ConsoleInput::TryGetKey() {
  auto c = TryGetChar();

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
  } else if (uc == 0x1b) {
    auto uc2 = static_cast<uint8_t>(TryGetChar().value_or(0));
    if (uc2 == 'O') {
      auto uc3 = static_cast<uint8_t>(TryGetChar().value_or(0));
      switch (uc3) {
      case 'P':
        return Key::Key_F1;
      case 'Q':
        return Key::Key_F2;
      case 'R':
        return Key::Key_F3;
      case 'S':
        return Key::Key_F4;
      default:
        assert(false);
      }
    } else if (uc2 == '[') {
      auto uc3 = static_cast<uint8_t>(TryGetChar().value_or(0));
      if (uc3 == 'A') {
        return Key::Key_Up;
      } else if (uc3 == 'B') {
        return Key::Key_Down;
      } else if (uc3 == 'C') {
        return Key::Key_Right;
      } else if (uc3 == 'D') {
        return Key::Key_Left;
      } else {
        auto c4 = TryGetChar();
        auto c5 = TryGetChar();
        if (c4.has_value() == true || c5.value_or(0) == '~') {
          auto uc4 = static_cast<uint8_t>(c4.value());
          if (uc3 == '1' && uc4 == '5') {
            return Key::Key_F5;
          } else if (uc3 == '1' && uc4 == '7') {
            return Key::Key_F6;
          } else if (uc3 == '1' && uc4 == '8') {
            return Key::Key_F7;
          } else if (uc3 == '1' && uc4 == '9') {
            return Key::Key_F8;
          } else if (uc3 == '2' && uc4 == '0') {
            return Key::Key_F9;
          } else if (uc3 == '2' && uc4 == '1') {
            return Key::Key_F10;
          } else if (uc3 == '2' && uc4 == '3') {
            return Key::Key_F11;
          } else if (uc3 == '2' && uc4 == '4') {
            return Key::Key_F12;
          }
        } else {
          assert(false);
        }
      }
    } else {
      assert(false);
    }
  }

  assert(false);
  return std::nullopt;
}

void ConsoleInput::FlushInputBuffer() {
  char c;
  while (read(STDIN_FILENO, &c, 1) == 1) {
  }
}
#endif
