#include <chrono>
#include <iostream>
#include <cstdint>
#include <print>
#include <thread>

#include "console_input.h"

auto main() -> int32_t {
  ConsoleInput input;

  for (;;) {
    auto key = input.NonblockingGetChar();
    if (key) {
      std::println("");
      std::println("key: {}", key.value());
      if (key.value() == 'q') {
        break;
      }
    } else {
      std::print(".");
      std::flush(std::cout);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
}
