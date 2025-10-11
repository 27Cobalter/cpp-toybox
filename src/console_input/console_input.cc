#ifdef _WIN32
#include <Windows.h>

#include <conio.h>
#else
#include <unistd.h>
#include <termios.h>
#endif

#include "console_input.h"


#ifdef _WIN32
ConsoleInput::ConsoleInput() {}
ConsoleInput::~ConsoleInput() {}

std::optional<char> ConsoleInput::NonblockingGetChar() {
  if (_kbhit() == true) {
    return static_cast<char>(_getch());
  } else{
    return std::nullopt;
  }
}
#else
ConsoleInput::ConsoleInput() {
  termios* old = new termios();
  context_ = reinterpret_cast<void*>(old);

  tcgetattr(STDIN_FILENO, old);
  termios tnew = *old;
  tnew.c_lflag &= ~(ICANON | ECHO);
  tnew.c_cc[VMIN] = 0;
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
    return c;
  }
  return std::nullopt;
}
#endif
