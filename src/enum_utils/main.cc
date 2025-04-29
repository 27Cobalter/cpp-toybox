#include <cstdint>
#include <iostream>

#include "enum_utils.h"

enum Enum : uint32_t {
  Ox00000000 = 0x00000000,
  Ox00000001 = 0x00000001,
  Ox00000002 = 0x00000002,
  Ox00000003 = 0x00000003,
  Ox00000004 = 0x00000004,
  Ox00000005 = 0x00000005,
  Ox00000006 = 0x00000006,
  Ox00000007 = 0x00000007,
  Ox00000008 = 0x00000008,
  Ox00000009 = 0x00000009,
  Ox0000000A = 0x0000000A,
  Ox0000000B = 0x0000000B,
  Ox0000000C = 0x0000000C,
  Ox0000000D = 0x0000000D,
  Ox0000000E = 0x0000000E,
  Ox0000000F = 0x0000000F,
  Ox00000010 = 0x00000010,
  Ox00000100 = 0x00000100,
  Ox00001000 = 0x00001000,
  Ox00010000 = 0x00010000,
  Ox00100000 = 0x00100000,
  Ox01000000 = 0x01000000,
  Ox10000000 = 0x10000000,
  Ox80000000 = 0x80000000,
  Ox80000001 = 0x80000001,
  Ox80000002 = 0x80000002,
  Ox80000003 = 0x80000003,
  Ox80000004 = 0x80000004,
  Ox80000005 = 0x80000005,
  Ox80000006 = 0x80000006,
  Ox80000007 = 0x80000007,
  Ox80000008 = 0x80000008,
  Ox80000009 = 0x80000009,
  Ox8000000A = 0x8000000A,
  Ox8000000B = 0x8000000B,
  Ox8000001C = 0x8000000C,
  Ox8000000D = 0x8000000D,
  Ox8000000E = 0x8000000E,
  Ox8000000F = 0x8000000F,
  OxFFFFFFFF = 0xFFFFFFFF,
};

template<>
struct EnumRanges<Enum> {
  static constexpr bool is_flag               = true;
  static constexpr std::array sequence_ranges = {
      EnumRange{0x00000000, 0x000000FF},
      EnumRange{0x80000000, 0x800000FF},
  };
};

auto main() -> int32_t {
  std::cout << std::format("FullName => {}", std::string{ValueName<Enum::Ox00000001>()}) << std::endl;
  std::cout << std::format("FullName => {}", std::string{ValueName<Enum::OxFFFFFFFF>()}) << std::endl;
  std::cout << std::format("FullName => {}", std::string{ValueName<static_cast<Enum>(0xF0F0F0F0)>()}) << std::endl;

  constexpr auto values = EnumValues<Enum>();
  std::cout << std::format("ValuesSize: {}", values.size()) << std::endl;
  for (const auto& v : values) {
    std::cout << std::format("Value => {:08X}", static_cast<uint32_t>(v)) << std::endl;
  }

  constexpr auto names = EnumNames<Enum>();
  std::cout << std::format("NamesSize: {}", names.size()) << std::endl;
  for (const auto& n : names) {
    std::cout << std::format("Names => {}", std::string{n}) << std::endl;
  }

  constexpr auto entries = EnumEntries<Enum>();
  std::cout << std::format("EntriesSize: {}", entries.size()) << std::endl;
  for (const auto& [value, name] : entries) {
    std::cout << std::format("Entries => {:08X}: {}", static_cast<uint32_t>(value), std::string{name}) << std::endl;
  }

  std::cout << std::format("EnumName => {}", EnumName<Enum>(Enum::Ox00000001)) << std::endl;
  std::cout << std::format("EnumName => {}", EnumName<Enum>(Enum::Ox80000001)) << std::endl;
  std::cout << std::format("EnumValue => {}", static_cast<uint32_t>(EnumCast<Enum>(0x00000001).value())) << std::endl;
  std::cout << std::format("EnumValue => {}", static_cast<uint32_t>(EnumCast<Enum>(0x80000001).value())) << std::endl;
  std::cout << std::format("EnumValue => {}", static_cast<uint32_t>(EnumCast<Enum>("Ox00000001").value())) << std::endl;
  std::cout << std::format("EnumValue => {}", static_cast<uint32_t>(EnumCast<Enum>("Ox80000001").value())) << std::endl;
  std::cout << std::format("EnumName => {}", EnumName<Enum>(static_cast<Enum>(0x20000001))) << std::endl;
  std::cout << std::format("EnumName => {}", EnumName<Enum>(static_cast<Enum>(0x70000001))) << std::endl;
  std::cout << std::format("EnumValue => {:08X}",
                           static_cast<uint32_t>(EnumCast<Enum>(0x20000001).value_or(Enum::OxFFFFFFFF)))
            << std::endl;
  std::cout << std::format("EnumValue => {:08X}",
                           static_cast<uint32_t>(EnumCast<Enum>(0x70000001).value_or(Enum::OxFFFFFFFF)))
            << std::endl;
  std::cout << std::format("EnumValue => {:08X}",
                           static_cast<uint32_t>(EnumCast<Enum>("Ox20000001").value_or(Enum::OxFFFFFFFF)))
            << std::endl;
  std::cout << std::format("EnumValue => {:08X}",
                           static_cast<uint32_t>(EnumCast<Enum>("Ox70000001").value_or(Enum::OxFFFFFFFF)))
            << std::endl;

  std::cout << std::format("EnumContains => {}", EnumContains<Enum>("Ox00000001")) << std::endl;
  std::cout << std::format("EnumContains => {}", EnumContains<Enum>("Ox80000001")) << std::endl;
  std::cout << std::format("EnumContains => {}", EnumContains<Enum>(0x00000001)) << std::endl;
  std::cout << std::format("EnumContains => {}", EnumContains<Enum>(0x80000001)) << std::endl;
  std::cout << std::format("EnumContains => {}", EnumContains<Enum>("Ox20000001")) << std::endl;
  std::cout << std::format("EnumContains => {}", EnumContains<Enum>("Ox70000001")) << std::endl;
  std::cout << std::format("EnumContains => {}", EnumContains<Enum>(0x20000001)) << std::endl;
  std::cout << std::format("EnumContains => {}", EnumContains<Enum>(0x70000001)) << std::endl;
  return 0;
}
