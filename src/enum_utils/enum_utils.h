#include <algorithm>
#include <array>
#include <cstdint>
#include <optional>
#include <ranges>
#include <source_location>
#include <string_view>
#include <type_traits>
#include <utility>

template<auto EV>
  requires std::is_enum_v<decltype(EV)>
consteval std::basic_string_view<char> ValueName() {
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
  requires std::is_enum_v<decltype(EV)>
constexpr auto ValuePredicate() {
  return ValueName<EV>() != "" ? std::optional<decltype(EV)>(EV) : std::nullopt;
};

struct EnumRange {
  uint32_t min;
  uint32_t max;
};

template<typename T>
  requires std::is_enum_v<T>
struct EnumRanges {
  static constexpr bool is_flag               = false;
  static constexpr std::array sequence_ranges = {
      EnumRange{0x00000000, 0x000000FF},
  };
};

template<typename T, uint32_t Min, size_t... Indices>
  requires std::is_enum_v<T>
consteval auto CreateEnumList_Sequence(std::index_sequence<Indices...>) {
  constexpr std::array values = {(ValuePredicate<static_cast<T>(Min + Indices)>())...};
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
  requires std::is_enum_v<T>
consteval auto CreateEnumList_Sequence_Invoke() {
  static_assert(Max - Min + 1 >= 1);
  return CreateEnumList_Sequence<T, Min>(std::make_index_sequence<Max - Min + 1>{});
}

template<typename T, size_t... Indices>
  requires std::is_enum_v<T>
consteval auto CreateEnumList_Bitflag(std::index_sequence<Indices...>) {
  constexpr std::array values = {(ValuePredicate<static_cast<T>(1 << Indices)>())...};
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
  requires std::is_enum_v<T>
consteval auto Concatenate(const std::array<T, Indices>&... arrays) {
  constexpr size_t total_size = (Indices + ... + 0);
  std::array<T, total_size> result{};
  size_t index = 0;
  ((std::copy(arrays.begin(), arrays.end(), result.begin() + index), index += arrays.size()), ...);
  return result;
}

template<typename T, size_t... Indices>
  requires std::is_enum_v<T>
consteval auto CreateEnumList_Sequences(std::index_sequence<Indices...>) {
  constexpr auto& ranges = EnumRanges<T>::sequence_ranges;
  return Concatenate<T>(CreateEnumList_Sequence_Invoke<T, ranges[Indices].min, ranges[Indices].max>()...);
}

template<typename T, size_t N>
  requires std::is_enum_v<T>
consteval auto UniqueSort(std::array<T, N> merged_list) {
  std::sort(merged_list.begin(), merged_list.end());
  auto duplicate = std::unique(merged_list.begin(), merged_list.end());
  auto distance  = std::distance(merged_list.begin(), duplicate);
  return std::tuple<decltype(merged_list), size_t>{merged_list, distance};
}

template<typename T>
  requires std::is_enum_v<T>
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
  requires std::is_enum_v<T>
consteval auto EnumNames_Impl(std::index_sequence<Indices...>) {
  constexpr std::array names = {(ValueName<static_cast<T>(values[Indices])>())...};
  return names;
}

template<typename T>
  requires std::is_enum_v<T>
consteval auto EnumNames() {
  constexpr auto values = EnumValues<T>();
  constexpr auto names  = EnumNames_Impl<T, values.size(), values>(std::make_index_sequence<values.size()>{});
  return names;
}

template<typename T>
  requires std::is_enum_v<T>
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
  requires std::is_enum_v<T>
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
  requires std::is_enum_v<T>
consteval size_t EnumCount() {
  constexpr auto values  = EnumValues<T>();
  constexpr size_t count = values.size();
  return count;
}

template<typename T>
  requires std::is_enum_v<T>
constexpr std::optional<T> EnumCast(std::string_view name) {
  constexpr auto entries = EnumEntries<T>();
  for (const auto& [value, entry_name] : entries) {
    if (name == entry_name) {
      return value;
    }
  }
  return std::nullopt;
}

template<typename T, std::integral U>
  requires std::is_enum_v<T>
constexpr std::optional<T> EnumCast(U value) {
  constexpr auto entries = EnumEntries<T>();
  for (const auto& [entry_value, entry_name] : entries) {
    if (static_cast<T>(value) == entry_value) {
      return entry_value;
    }
  }
  return std::nullopt;
}

template<typename T>
  requires std::is_enum_v<T>
constexpr bool EnumContains(std::string_view name) {
  constexpr auto names = EnumNames<T>();
  for (const auto& entry_name : names) {
    if (name == entry_name) {
      return true;
    }
  }
  return false;
}

template<typename T, std::integral U>
  requires std::is_enum_v<T>
constexpr bool EnumContains(U value) {
  constexpr auto values = EnumValues<T>();
  for (const auto& entry_value : values) {
    if (static_cast<T>(value) == entry_value) {
      return true;
    }
  }
  return false;
}
