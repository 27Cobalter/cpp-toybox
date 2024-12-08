#include <chrono>
#include <fstream>
#include <iostream>
#include <ranges>
#include <thread>

auto main() -> int {
  constexpr int32_t buffer_size = 16;
  char buffer[buffer_size];

  std::istream* is;
  std::ostream* os;
  is = &std::cin;
  os = &std::cout;

  while (is->eof() == false) {
    // auto read_size = is->readsome(buffer, buffer_size);
    is->read(buffer, buffer_size);
    auto read_size = is->gcount();
    std::cout << read_size << std::endl;
    if (read_size == 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
      continue;
    }
    for (auto i : std::views::iota(0, read_size)) {
      buffer[i] += 'A';
    }
    std::cout.write(buffer, read_size);
    std::cout << std::endl;
  }
  std::cout << std::endl;

  return 0;
}
