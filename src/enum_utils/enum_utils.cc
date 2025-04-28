#include <algorithm>
#include <array>
#include <cstdint>
#include <format>
#include <iostream>
#include <optional>
#include <ranges>
#include <source_location>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

template<auto EV>
  requires std::is_enum_v<decltype(EV)>
consteval std::string_view ValueName() {
  auto location      = std::source_location::current();
  auto function_name = std::string_view(location.function_name());
#if _MSC_VER
  std::string_view prefix = "ValueName<";
  std::string_view suffix = ">(";
#else
  std::string_view prefix = "EV = ";
  std::string_view suffix = "]";
#endif
  int32_t start            = function_name.find(prefix) + prefix.length();
  int32_t end              = function_name.rfind(suffix) - start;
  std::string_view content = function_name.substr(start, end);

  std::string_view separator = "::";
  if (auto pos = content.rfind(separator); pos != std::string_view::npos) {
    content.remove_prefix(pos + separator.length());
  }

  if (content.front() != '(') {
    return content;
  } else {
    return "";
  }
}

template<auto EV>
struct ValuePredicate {
  static constexpr auto value = ValueName<EV>() != "" ? std::optional<decltype(EV)>(EV) : std::nullopt;
};

struct EnumRange {
  uint32_t min;
  uint32_t max;
};

template<typename T>
struct EnumRanges {
  static constexpr bool is_flag               = false;
  static constexpr std::array sequence_ranges = {
      EnumRange{0x00000000, 0x000000FF},
  };
};

template<typename T, uint32_t Min, size_t... Indices>
consteval auto CreateEnumList_Sequence(std::index_sequence<Indices...>) {
  constexpr std::array values = {(ValuePredicate<static_cast<T>(Min + Indices)>::value)...};
  constexpr size_t count      = ((values[Indices].has_value()) + ... + 0);
  std::array<T, count> result{};
  size_t index = 0;
  for (size_t i = 0; i < values.size(); ++i) {
    if (values[i].has_value()) {
      result[index++] = values[i].value();
    }
  }
  return result;
}

template<typename T, auto Min, auto Max>
consteval auto CreateEnumList_Sequence_Invoke() {
  static_assert(Max - Min + 1 >= 1);
  return CreateEnumList_Sequence<T, Min>(std::make_index_sequence<Max - Min + 1>{});
}

template<typename T, size_t... Indices>
consteval auto CreateEnumList_Bitflag(std::index_sequence<Indices...>) {
  constexpr std::array values = {(ValuePredicate<static_cast<T>(1 << Indices)>::value)...};
  constexpr size_t count      = ((values[Indices].has_value()) + ... + 0);
  std::array<T, count> result{};
  size_t index = 0;
  for (size_t i = 0; i < values.size(); ++i) {
    if (values[i].has_value()) {
      result[index++] = values[i].value();
    }
  }
  return result;
}

template<typename T, size_t... Indices>
consteval auto Concatenate(const std::array<T, Indices>&... arrays) {
  constexpr size_t total_size = (Indices + ... + 0);
  std::array<T, total_size> result{};
  size_t index = 0;
  ((std::copy(arrays.begin(), arrays.end(), result.begin() + index), index += arrays.size()), ...);
  return result;
}

template<typename T, size_t... Indices>
consteval auto CreateEnumList_Sequences(std::index_sequence<Indices...>) {
  constexpr auto& ranges = EnumRanges<T>::sequence_ranges;
  return Concatenate<T>(CreateEnumList_Sequence_Invoke<T, ranges[Indices].min, ranges[Indices].max>()...);
}

template<typename T, size_t N>
consteval auto UniqueSort(std::array<T, N> merged_list) {
  std::sort(merged_list.begin(), merged_list.end());
  auto duplicate = std::unique(merged_list.begin(), merged_list.end());
  auto distance  = std::distance(merged_list.begin(), duplicate);
  return std::tuple<decltype(merged_list), size_t>{merged_list, distance};
}

template<typename T>
consteval auto EnumValues() {
  constexpr auto& ranges      = EnumRanges<T>::sequence_ranges;
  constexpr std::array result = CreateEnumList_Sequences<T>(std::make_index_sequence<ranges.size()>{});

  if constexpr (EnumRanges<T>::is_flag == false) {
    return result;
  } else {
    constexpr std::array flag_list = CreateEnumList_Bitflag<T>(std::make_index_sequence<31>{});
    constexpr auto merged_list     = Concatenate<T>(result, flag_list);
    constexpr auto result          = UniqueSort<T>(merged_list);
    std::array<T, std::get<1>(result)> ret_list;
    std::copy(std::get<0>(result).begin(), std::get<0>(result).begin() + std::get<1>(result), ret_list.begin());
    return ret_list;
  }
}

template<typename T, size_t N, std::array<T, N> values, size_t... Indices>
consteval auto EnumNames_Impl(std::index_sequence<Indices...>) {
  constexpr std::array names = {(ValueName<static_cast<T>(values[Indices])>())...};
  return names;
}

template<typename T>
consteval auto EnumNames() {
  constexpr auto values = EnumValues<T>();
  constexpr auto names  = EnumNames_Impl<T, values.size(), values>(std::make_index_sequence<values.size()>{});
  return names;
}

template<typename T>
consteval auto EnumEntries() {
  constexpr auto values = EnumValues<T>();
  constexpr auto names  = EnumNames<T>();
  std::array<std::pair<T, std::string_view>, values.size()> entries{};
  for (size_t i = 0; i < values.size(); ++i) {
    entries[i] = {values[i], names[i]};
  }
  return entries;
}

template<typename T>
constexpr auto EnumName(T value) {
  constexpr auto entries = EnumEntries<T>();
  for (const auto& [entry_value, entry_name] : entries) {
    if (value == entry_value) {
      return entry_name;
    }
  }
  return std::string_view{};
}

template<typename T>
consteval size_t EnumCount() {
  constexpr auto values = EnumValues<T>();
  constexpr size_t count = values.size();
  return count;
}

template<typename T>
constexpr std::optional<T> EnumCast(std::string_view name) {
  constexpr auto entries = EnumEntries<T>();
  for (const auto& [value, entry_name] : entries) {
    if (name == entry_name) {
      return value;
    }
  }
  return std::nullopt;
}

template<typename T>
constexpr std::optional<T> EnumCast(uint32_t value) {
  constexpr auto entries = EnumEntries<T>();
  for (const auto& [entry_value, entry_name] : entries) {
    if (value == static_cast<uint32_t>(entry_value)) {
      return entry_value;
    }
  }
  return std::nullopt;
}

template<typename T>
constexpr bool EnumContains(std::string_view name) {
  constexpr auto names = EnumNames<T>();
  for (const auto& entry_name : names) {
    if (name == entry_name) {
      return true;
    }
  }
  return false;
}

template<typename T>
constexpr bool EnumContains(uint32_t value) {
  constexpr auto values = EnumValues<T>();
  for (const auto& entry_value : values) {
    if (value == entry_value) {
      return true;
    }
  }
  return false;
}

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
