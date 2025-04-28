#include <magic_enum/magic_enum.hpp>
#include <iostream>

// https://github.com/Neargye/magic_enum?tab=readme-ov-file#features--examples
// https://github.com/Neargye/magic_enum/blob/master/doc/limitations.md

enum Enum : uint32_t {
  Ox00000001 = 0x00000001,
  Ox00000010 = 0x00000010,
  Ox00000100 = 0x00000100,
  Ox00001000 = 0x00001000,
  Ox00010000 = 0x00010000,
  Ox00100000 = 0x00100000,
  Ox01000000 = 0x01000000,
  Ox10000000 = 0x10000000,
  OxFFFFFFFF = 0xFFFFFFFF,
};

int main() {
  std::cout << magic_enum::enum_name(Enum::Ox00000001) << std::endl;
  std::cout << magic_enum::enum_name(Enum::Ox00000010) << std::endl;
  std::cout << magic_enum::enum_name(Enum::Ox00000100) << std::endl;
  std::cout << magic_enum::enum_name(Enum::Ox00001000) << std::endl;
  std::cout << magic_enum::enum_name(Enum::Ox00010000) << std::endl;
  std::cout << magic_enum::enum_name(Enum::Ox00100000) << std::endl;
  std::cout << magic_enum::enum_name(Enum::Ox01000000) << std::endl;
  std::cout << magic_enum::enum_name(Enum::Ox10000000) << std::endl;
  std::cout << magic_enum::enum_name(Enum::OxFFFFFFFF) << std::endl;
  return 0;
}
