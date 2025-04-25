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
consteval std::basic_string_view<char> FullName() {
  auto loc   = std::source_location::current();
  auto fname = std::string_view(loc.function_name());
#if _MSC_VER
  std::string_view prefix = "FullName<";
  std::string_view suffix = ">(";
#else
  std::string_view prefix = "EV = ";
  std::string_view suffix = "]";
#endif
  int32_t start = fname.find(prefix) + prefix.length();
  int32_t end   = fname.rfind(suffix) - start;
  return fname.substr(start, end);
}

template <auto EV>
  requires std::is_enum_v<decltype(EV)>
consteval std::string_view ValueName() {
  std::string_view aa = FullName<EV>();
  std::vector<std::string_view> sv;
  using namespace std::literals;
  for (const auto& e : aa | std::ranges::views::split("::"sv)) {
    sv.emplace_back(std::string_view{e.begin(), e.end()});
  }
  auto back = sv.back();

  if (back.front() != '(') {
    return back;
  } else {
    return "";
  }
}

template <auto EV>
struct GetName {
  static constexpr std::string_view name = ValueName<EV>();
  static constexpr auto value =
      name != "" ? std::optional<std::string_view>(name) : std::nullopt;
};

template <auto EV>
struct GetValue {
  static constexpr auto value =
      ValueName<EV>() != "" ? std::optional<decltype(EV)>(EV) : std::nullopt;
};

template <typename T, uint32_t Min, uint32_t Max, template <auto> typename Predicate,
          typename ResultType, size_t... Indices>
consteval auto create_filtered_list_impl(std::index_sequence<Indices...>) {
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

template <typename T, auto Min, auto Max, template <auto> typename Predicate,
          typename ResultType>
consteval auto create_filtered_list() {
  return create_filtered_list_impl<T, Min, Max, Predicate, ResultType>(
      std::make_index_sequence<Max - Min + 1>{});
}

template <size_t size, typename T, std::size_t... sizes>
consteval auto concatenate(const std::array<T, sizes>&... arrays) {
  std::array<T, size> result{};
  size_t index = 0;
  ((std::copy(arrays.begin(), arrays.end(), result.begin() + index), index += arrays.size()),
   ...);
  return result;
}

template <typename T, auto Min, auto Max, template <auto> typename Predicate,
          typename ResultType>
consteval auto create_filtered_tree() {
  constexpr auto Half = Max / 2;
  constexpr auto lo   = create_filtered_list<T, Min, Half, Predicate, ResultType>();
  constexpr auto hi   = create_filtered_list<T, Half + 1, Max, Predicate, ResultType>();
  constexpr auto size = lo.size() + hi.size();
  return concatenate<size>(lo, hi);
}

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

auto main() -> int32_t {
  std::println("FullName => {}", std::string{ValueName<Enum::Ox00000001>()});
  std::println("FullName => {}", std::string{ValueName<Enum::OxFFFFFFFF>()});
  std::println("FullName => {}", std::string{ValueName<static_cast<Enum>(0xF0F0F0F0)>()});

  constexpr auto filtered_list =
      create_filtered_tree<Enum, 0x00000000, 0x00000100, GetName, std::string_view>();
  std::println("Filtered list size: {}", filtered_list.size());
  for (const auto& e : filtered_list) {
    std::println("Filtered => {}", std::string(e));
  }

  // constexpr auto filtered_value =
  //     create_filtered_list<Enum, 0x00000000, 0x00001100, GetValue, Enum>();
  // std::println("Filtered list size: {}", filtered_value.size());
  // for (const auto& e : filtered_value) {
  //   std::println("Filtered => {:08X}", static_cast<int32_t>(e));
  // }
  return 0;
}
