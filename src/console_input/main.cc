#include <chrono>
#include <iostream>
#include <cstdint>
#include <print>
#include <thread>

#include "enum_utils.h"
#include "console_input.h"

auto main() -> int32_t {
  ConsoleInput input;

  for (;;) {
    auto key = input.NonBlockingGetKey();
    if (key) {
      std::println("");
      std::println("key: {}", EnumName(key.value()));
      if (key.value() == Key::Key_Q) {
        break;
      }
    } else {
      std::print(".");
      std::flush(std::cout);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  }
}
