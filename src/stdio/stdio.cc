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

  std::istream* is;
  std::ostream* os;
  is = &std::cin;
  os = &std::cout;

  while (is->eof() == false) {
    is->read(buffer, buffer_size);
    auto read_size = is->gcount();
    std::cout << read_size << std::endl;
    for(auto i : std::views::iota(0, read_size))
    {
      buffer[i] += 'A';
    }
    std::cout.write(buffer, read_size);
  }
  std::cout << std::endl;

  return 0;
}
