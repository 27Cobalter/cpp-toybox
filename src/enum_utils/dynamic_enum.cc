#include <cstdint>
#include <optional>
#include <print>
#include <ranges>
#include <source_location>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

template <auto EV>
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

template <auto EV>
struct GetNamePredicate {
  static constexpr std::string_view name = ValueName<EV>();
  static constexpr auto value =
      name != "" ? std::optional<std::string_view>(name) : std::nullopt;
};

template <auto EV>
struct GetValuePredicate {
  static constexpr auto value =
      ValueName<EV>() != "" ? std::optional<decltype(EV)>(EV) : std::nullopt;
};

struct EnumRange {
  uint32_t min;
  uint32_t max;
};

template <typename T>
struct EnumRanges {
  static constexpr bool is_flag      = false;
  static constexpr std::array ranges = {
      EnumRange{0x00000000, 0x000000FF},
  };
};

template <typename T, uint32_t Min, template <auto> typename Predicate, typename ResultType,
          size_t... Indices>
consteval auto GetEnumList_Sequence(std::index_sequence<Indices...>) {
  constexpr std::array values = {(Predicate<static_cast<T>(Min + Indices)>::value)...};

  constexpr size_t count = ((values[Indices].has_value()) + ...);
  std::array<ResultType, count> result{};
  size_t index = 0;
  for (size_t i = 0; i < values.size(); ++i) {
    if (values[i].has_value()) {
      result[index++] = values[i].value();
    }
  }

  return result;
}

template <typename T, template <auto> typename Predicate, typename ResultType,
          size_t... Indices>
consteval auto GetEnumList_Bitflag(std::index_sequence<Indices...>) {
  constexpr std::array values = {(Predicate<static_cast<T>(1 << Indices)>::value)...};

  constexpr size_t count = ((values[Indices].has_value()) + ...);
  std::array<ResultType, count> result{};
  size_t index = 0;
  for (size_t i = 0; i < values.size(); ++i) {
    if (values[i].has_value()) {
      result[index++] = values[i].value();
    }
  }

  return result;
}

template <typename T, auto Min, auto Max, template <auto> typename Predicate,
          typename ResultType>
consteval auto GetEnumList_Invoke() {
  static_assert(Max - Min + 1 >= 1);
  return GetEnumList_Sequence<T, Min, Predicate, ResultType>(
      std::make_index_sequence<Max - Min + 1>{});

  // return GetEnumList_Bitflag<T, Predicate, ResultType>(std::make_index_sequence<31>{});
}

template <typename ResultType, size_t size, std::size_t... sizes>
consteval auto Concatenate(const std::array<ResultType, sizes>&... arrays) {
  std::array<ResultType, size> result{};
  size_t index = 0;
  ((std::copy(arrays.begin(), arrays.end(), result.begin() + index), index += arrays.size()),
   ...);
  return result;
}

template <typename T, auto Min, auto Max, template <auto> typename Predicate,
          typename ResultType>
consteval auto GetEnumList() {
  constexpr auto Half = (Max - Min) / 2 + Min;
  if constexpr (Max - Min < 256) {
    return GetEnumList_Invoke<T, Min, Max, Predicate, ResultType>();
  } else {
    constexpr auto lo   = GetEnumList<T, Min, Half, Predicate, ResultType>();
    constexpr auto hi   = GetEnumList<T, Half + 1, Max, Predicate, ResultType>();
    constexpr auto size = lo.size() + hi.size();
    return Concatenate<ResultType, size>(lo, hi);
  }
}

template <typename T, template <auto> typename Predicate, typename ResultType, size_t... I>
consteval auto GetEnumList_FromRanges_Impl(std::index_sequence<I...>) {
  constexpr auto& ranges = EnumRanges<T>::ranges;
  constexpr size_t total_size =
      (GetEnumList<T, ranges[I].min, ranges[I].max, Predicate, ResultType>().size() + ...);
  return Concatenate<ResultType, total_size>(
      GetEnumList<T, ranges[I].min, ranges[I].max, Predicate, ResultType>()...);
}

template <typename T, template <auto> typename Predicate, typename ResultType>
consteval auto GetEnumList_AllRanges() {
  constexpr auto& ranges = EnumRanges<T>::ranges;
  return GetEnumList_FromRanges_Impl<T, Predicate, ResultType>(
      std::make_index_sequence<ranges.size()>{});
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

template <>
struct EnumRanges<Enum> {
  static constexpr std::array ranges = {
      EnumRange{0x00000000, 0x000000FF},
      EnumRange{0x80000000, 0x800000FF},
  };
};

auto main() -> int32_t {
  std::println("FullName => {}", std::string{ValueName<Enum::Ox00000001>()});
  std::println("FullName => {}", std::string{ValueName<Enum::OxFFFFFFFF>()});
  std::println("FullName => {}", std::string{ValueName<static_cast<Enum>(0xF0F0F0F0)>()});

  constexpr auto filtered_list =
      GetEnumList<Enum, 0x00000000, 0x000000FF, GetNamePredicate, std::string_view>();
  std::println("Filtered list size: {}", filtered_list.size());
  for (const auto& e : filtered_list) {
    std::println("Filtered => {}", std::string(e));
  }

  constexpr auto filtered_value =
      GetEnumList<Enum, 0x00000000, 0x000000FF, GetValuePredicate, Enum>();
  std::println("Filtered list size: {}", filtered_value.size());
  for (const auto& e : filtered_value) {
    std::println("Filtered => {:08X}", static_cast<int32_t>(e));
  }

  constexpr auto filtered_all_list =
      GetEnumList_AllRanges<Enum, GetNamePredicate, std::string_view>();
  std::println("Filtered all ranges list size: {}", filtered_all_list.size());
  for (const auto& e : filtered_all_list) {
    std::println("Filtered all ranges => {}", std::string(e));
  }
  return 0;
}
