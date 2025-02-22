#include <concepts>
#include <limits>
#include <ranges>
#include <source_location>
#include <string_view>
#include <type_traits>
#include <vector>
#include <version>

#include <print>

namespace enum_utils {
template <auto T>
concept IsEnumValue = std::is_enum_v<decltype(T)>;

#ifdef __cpp_lib_is_scoped_enum
template <auto T>
concept IsScopedEnumValue = std::is_scoped_enum_v<decltype(T)>;
#else
template <auto T>
concept IsScopedEnumValue = requires {
  requires std::is_enum_v<decltype(T)>;
  requires !std::is_convertible_v<decltype(T), std::underlying_type_t<decltype(T)>>;
};
#endif

template <auto EV>
  requires IsEnumValue<EV>
consteval std::basic_string_view<char> RawName() {
  auto loc   = std::source_location::current();
  auto fname = std::string_view(loc.function_name());
  return fname;
}

template <auto EV>
  requires IsEnumValue<EV>
consteval std::basic_string_view<char> FullName() {
  auto loc                = std::source_location::current();
  auto fname              = std::string_view(loc.function_name());
  std::string_view prefix = "EV = ";
  std::string_view suffix = "]";
  int32_t start           = fname.find(prefix) + prefix.length();
  int32_t end             = fname.rfind(suffix) - start;
  return fname.substr(start, end);
}

template <auto EV>
  requires IsScopedEnumValue<EV>
consteval std::basic_string_view<char> ScopedName() {
  std::string_view aa = enum_utils::FullName<EV>();
  std::vector<std::string_view> sv;
  using namespace std::literals;
  for (auto e : aa | std::ranges::views::split("::"sv)) {
    sv.emplace_back(std::string_view{e.begin(), e.end()});
  }
  if (sv.size() >= 2) {
    return std::string_view(sv[sv.size() - 2].begin(), sv[sv.size() - 1].end());
  } else {
    return "";
  }
}

template <auto EV>
  requires IsScopedEnumValue<EV>
consteval std::basic_string_view<char> TypeName() {
  std::string_view aa = enum_utils::FullName<EV>();
  std::vector<std::string_view> sv;
  using namespace std::literals;
  for (auto e : aa | std::ranges::views::split("::"sv)) {
    sv.emplace_back(std::string_view{e.begin(), e.end()});
  }
  if (sv.size() >= 2) {
    return sv[sv.size() - 2];
  } else {
    return "";
  }
}

template <auto EV>
  requires IsEnumValue<EV>
consteval std::string_view ValueName() {
  std::string_view aa = enum_utils::FullName<EV>();
  std::vector<std::string_view> sv;
  using namespace std::literals;
  for (auto e : aa | std::ranges::views::split("::"sv)) {
    sv.emplace_back(std::string_view{e.begin(), e.end()});
  }
  return sv.back();
}
namespace __TEST {
constexpr auto TEST_ENUM   = std::float_round_style::round_to_nearest;
constexpr auto TEST_SCOPED = std::ranges::subrange_kind::unsized;
static_assert(IsEnumValue<TEST_ENUM>);
static_assert(IsEnumValue<TEST_SCOPED>);
// static_assert(IsScopedEnumValue<TEST_ENUM>); // Err
static_assert(IsScopedEnumValue<TEST_SCOPED>);

static_assert(FullName<TEST_ENUM>() == "std::round_to_nearest");
// static_assert(ScopedName<TEST_ENUM>() == "float_round_style::round_to_nearest"); // Err
// static_assert(TypeName<TEST_ENUM>() == "float_round_type"); // Err
static_assert(ValueName<TEST_ENUM>() == "round_to_nearest");

static_assert(FullName<TEST_SCOPED>() == "std::ranges::subrange_kind::unsized");
static_assert(ScopedName<TEST_SCOPED>() == "subrange_kind::unsized");
static_assert(TypeName<TEST_SCOPED>() == "subrange_kind");
static_assert(ValueName<TEST_SCOPED>() == "unsized");
} // namespace __TEST
} // namespace enum_utils

enum GlobalC { Va, Vb };
enum class GlobalCpp { Vc, Vd };

namespace Scope {
enum ScopedC { Ve, Vf };
enum class ScopedCpp { Vg, Vh };

namespace Nest {
enum NestedC { Vi, Vj };
enum class NestedCpp { Vk, Vl };
} // namespace Nest
} // namespace Scope

class Class {
public:
  enum ClassC { Vm, Vn };
  enum class ClassCpp { Vo, Vp };
};

template <auto Front, auto... Args>
auto PrintTypeValue() {
  std::println("RawName => {}", enum_utils::RawName<Front>());
  std::println("FullName => {}", enum_utils::FullName<Front>());
  if constexpr (enum_utils::IsScopedEnumValue<Front>) {
    std::println("ScopedName => {}", enum_utils::ScopedName<Front>());
    std::println("TypeName => {}", enum_utils::TypeName<Front>());
  }
  std::println("ValueName => {}", enum_utils::ValueName<Front>());
  std::println();

  if constexpr (sizeof...(Args) > 0) PrintTypeValue<Args...>();
}

auto main() -> int {
  {
    PrintTypeValue<GlobalC::Va, GlobalCpp::Vc, Scope::ScopedC::Ve, Scope::ScopedCpp::Vg,
                   Scope::Nest::NestedC::Vi, Scope::Nest::NestedCpp::Vk, Class::ClassC::Vm,
                   Class::ClassCpp::Vo>(); // Full
  }

  {
    PrintTypeValue<Vb>();        // direct value
    PrintTypeValue<Scope::Vf>(); // namespace direct value

    using ScopedCpp = Scope::ScopedCpp;
    PrintTypeValue<ScopedCpp::Vh>(); // using short

    using namespace Scope::Nest;
    PrintTypeValue<Vj>();            // using namespace direct
    PrintTypeValue<NestedCpp::Vl>(); // using namespace

    using AnotherName = Class::ClassCpp;
    PrintTypeValue<AnotherName::Vp>(); // using rename
  }
  return 0;
}
