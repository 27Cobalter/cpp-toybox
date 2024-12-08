#include <iostream>
#include <fstream>
#include <ranges>

auto main() -> int {
  constexpr int32_t buffer_size = 16;
  char buffer[buffer_size];

  std::ofstream ofs("./bin.bin", std::ios::binary);
  for (auto i : std::views::iota(0, 26)) {
    char buf[1];
    buf[0] = i;
    ofs.write(buf, 1);
  }
  ofs.close();

  return 0;
}
