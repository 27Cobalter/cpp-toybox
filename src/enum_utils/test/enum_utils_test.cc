#include <gtest/gtest.h>

#include <array>
#include <ranges>

#include "enum_utils.h"

enum class U32CustomRange : uint32_t {
  Ox00000000 = 0x00000000,
  Ox00000001 = 0x00000001,
  Ox00000002 = 0x00000002,
  Ox00000003 = 0x00000003,
  Ox00000004 = 0x00000004,
  Ox00000005 = 0x00000005,
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
};

enum class U32DefaultRange : uint32_t {
  Ox00000000 = 0x00000000,
  Ox00000001 = 0x00000001,
  Ox00000002 = 0x00000002,
  Ox00000003 = 0x00000003,
  Ox00000004 = 0x00000004,
  Ox00000005 = 0x00000005,
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
};

enum class I32CustomRange : int32_t {
  Ox00000000 = static_cast<int32_t>(0x00000000),
  Ox00000001 = static_cast<int32_t>(0x00000001),
  Ox00000002 = static_cast<int32_t>(0x00000002),
  Ox00000003 = static_cast<int32_t>(0x00000003),
  Ox00000004 = static_cast<int32_t>(0x00000004),
  Ox00000005 = static_cast<int32_t>(0x00000005),
  Ox00000010 = static_cast<int32_t>(0x00000010),
  Ox00000100 = static_cast<int32_t>(0x00000100),
  Ox00001000 = static_cast<int32_t>(0x00001000),
  Ox00010000 = static_cast<int32_t>(0x00010000),
  Ox00100000 = static_cast<int32_t>(0x00100000),
  Ox01000000 = static_cast<int32_t>(0x01000000),
  Ox10000000 = static_cast<int32_t>(0x10000000),
  Ox80000000 = static_cast<int32_t>(0x80000000),
  Ox80000001 = static_cast<int32_t>(0x80000001),
  Ox80000002 = static_cast<int32_t>(0x80000002),
  Ox80000003 = static_cast<int32_t>(0x80000003),
  Ox80000004 = static_cast<int32_t>(0x80000004),
  Ox80000005 = static_cast<int32_t>(0x80000005),
};

enum class I32DefaultRange : int32_t {
  Ox00000000 = static_cast<int32_t>(0x00000000),
  Ox00000001 = static_cast<int32_t>(0x00000001),
  Ox00000002 = static_cast<int32_t>(0x00000002),
  Ox00000003 = static_cast<int32_t>(0x00000003),
  Ox00000004 = static_cast<int32_t>(0x00000004),
  Ox00000005 = static_cast<int32_t>(0x00000005),
  Ox00000010 = static_cast<int32_t>(0x00000010),
  Ox00000100 = static_cast<int32_t>(0x00000100),
  Ox00001000 = static_cast<int32_t>(0x00001000),
  Ox00010000 = static_cast<int32_t>(0x00010000),
  Ox00100000 = static_cast<int32_t>(0x00100000),
  Ox01000000 = static_cast<int32_t>(0x01000000),
  Ox10000000 = static_cast<int32_t>(0x10000000),
  Ox80000000 = static_cast<int32_t>(0x80000000),
  Ox80000001 = static_cast<int32_t>(0x80000001),
  Ox80000002 = static_cast<int32_t>(0x80000002),
  Ox80000003 = static_cast<int32_t>(0x80000003),
  Ox80000004 = static_cast<int32_t>(0x80000004),
  Ox80000005 = static_cast<int32_t>(0x80000005),
};

constexpr std::array ExpectValuesU32 = {
    static_cast<uint32_t>(0x00000000), static_cast<uint32_t>(0x00000001), static_cast<uint32_t>(0x00000002),
    static_cast<uint32_t>(0x00000003), static_cast<uint32_t>(0x00000004), static_cast<uint32_t>(0x00000005),
    static_cast<uint32_t>(0x00000010), static_cast<uint32_t>(0x00000100), static_cast<uint32_t>(0x00001000),
    static_cast<uint32_t>(0x00010000), static_cast<uint32_t>(0x00100000), static_cast<uint32_t>(0x01000000),
    static_cast<uint32_t>(0x10000000), static_cast<uint32_t>(0x80000000), static_cast<uint32_t>(0x80000001),
    static_cast<uint32_t>(0x80000002), static_cast<uint32_t>(0x80000003), static_cast<uint32_t>(0x80000004),
    static_cast<uint32_t>(0x80000005),
};
std::array ExpectNamesU32 = {
    "Ox00000000", "Ox00000001", "Ox00000002", "Ox00000003", "Ox00000004", "Ox00000005", "Ox00000010",
    "Ox00000100", "Ox00001000", "Ox00010000", "Ox00100000", "Ox01000000", "Ox10000000", "Ox80000000",
    "Ox80000001", "Ox80000002", "Ox80000003", "Ox80000004", "Ox80000005",
};
constexpr std::array ExpectValuesI32 = {
    static_cast<uint32_t>(0x80000000), static_cast<uint32_t>(0x80000001), static_cast<uint32_t>(0x80000002),
    static_cast<uint32_t>(0x80000003), static_cast<uint32_t>(0x80000004), static_cast<uint32_t>(0x80000005),
    static_cast<uint32_t>(0x00000000), static_cast<uint32_t>(0x00000001), static_cast<uint32_t>(0x00000002),
    static_cast<uint32_t>(0x00000003), static_cast<uint32_t>(0x00000004), static_cast<uint32_t>(0x00000005),
    static_cast<uint32_t>(0x00000010), static_cast<uint32_t>(0x00000100), static_cast<uint32_t>(0x00001000),
    static_cast<uint32_t>(0x00010000), static_cast<uint32_t>(0x00100000), static_cast<uint32_t>(0x01000000),
    static_cast<uint32_t>(0x10000000),
};
std::array ExpectNamesI32 = {
    "Ox80000000", "Ox80000001", "Ox80000002", "Ox80000003", "Ox80000004", "Ox80000005", "Ox00000000",
    "Ox00000001", "Ox00000002", "Ox00000003", "Ox00000004", "Ox00000005", "Ox00000010", "Ox00000100",
    "Ox00001000", "Ox00010000", "Ox00100000", "Ox01000000", "Ox10000000",
};

template<>
struct EnumRanges<U32CustomRange> {
  static constexpr bool is_flag               = true;
  static constexpr std::array sequence_ranges = {
      EnumRange{0x00000000, 0x000000FF},
      EnumRange{0x80000000, 0x800000FF},
  };
};
template<>
struct EnumRanges<I32CustomRange> {
  static constexpr bool is_flag               = true;
  static constexpr std::array sequence_ranges = {
      EnumRange{0x00000000, 0x000000FF},
      EnumRange{0x80000000, 0x800000FF},
  };
};

TEST(ENUM_UTILS, ValueName) {
  ASSERT_STREQ(std::string(ValueName<U32CustomRange::Ox00000001>()).c_str(), "Ox00000001");
  ASSERT_STREQ(std::string(ValueName<I32CustomRange::Ox00000001>()).c_str(), "Ox00000001");
  ASSERT_STREQ(std::string(ValueName<U32CustomRange::Ox80000001>()).c_str(), "Ox80000001");
  ASSERT_STREQ(std::string(ValueName<I32CustomRange::Ox80000001>()).c_str(), "Ox80000001");
  ASSERT_STREQ(std::string(ValueName<U32DefaultRange::Ox00000001>()).c_str(), "Ox00000001");
  ASSERT_STREQ(std::string(ValueName<I32DefaultRange::Ox00000001>()).c_str(), "Ox00000001");
  ASSERT_STREQ(std::string(ValueName<U32DefaultRange::Ox80000001>()).c_str(), "Ox80000001");
  ASSERT_STREQ(std::string(ValueName<I32DefaultRange::Ox80000001>()).c_str(), "Ox80000001");
}

TEST(ENUM_UTILS, ValuePredicate) {
  ASSERT_TRUE(ValuePredicate<U32CustomRange::Ox00000001>().has_value());
  ASSERT_TRUE(ValuePredicate<I32CustomRange::Ox00000001>().has_value());
  ASSERT_TRUE(ValuePredicate<U32CustomRange::Ox80000001>().has_value());
  ASSERT_TRUE(ValuePredicate<I32CustomRange::Ox80000001>().has_value());
  ASSERT_TRUE(ValuePredicate<U32DefaultRange::Ox00000001>().has_value());
  ASSERT_TRUE(ValuePredicate<I32DefaultRange::Ox00000001>().has_value());
  ASSERT_TRUE(ValuePredicate<U32DefaultRange::Ox80000001>().has_value());
  ASSERT_TRUE(ValuePredicate<I32DefaultRange::Ox80000001>().has_value());

  ASSERT_FALSE(ValuePredicate<static_cast<U32CustomRange>(0xF0000001)>().has_value());
  ASSERT_FALSE(ValuePredicate<static_cast<I32CustomRange>(0xF0000001)>().has_value());
  ASSERT_FALSE(ValuePredicate<static_cast<U32DefaultRange>(0xF0000001)>().has_value());
  ASSERT_FALSE(ValuePredicate<static_cast<I32DefaultRange>(0xF0000001)>().has_value());
}

TEST(ENUM_UTILS, EnumValues) {
  constexpr auto U32CustomList  = EnumValues<U32CustomRange>();
  constexpr auto I32CustomList  = EnumValues<I32CustomRange>();
  constexpr auto U32DefaultList = EnumValues<U32DefaultRange>();
  constexpr auto I32DefaultList = EnumValues<I32DefaultRange>();

  ASSERT_EQ(19, U32CustomList.size());
  ASSERT_EQ(19, I32CustomList.size());
  ASSERT_EQ(7, U32DefaultList.size());
  ASSERT_EQ(7, I32DefaultList.size());

  for (auto i : std::views::iota(0u, U32CustomList.size())) {
    ASSERT_EQ(ExpectValuesU32[i], static_cast<uint32_t>(U32CustomList[i]));
    ASSERT_EQ(ExpectValuesI32[i], static_cast<int32_t>(I32CustomList[i]));
  }
  for (auto i : std::views::iota(0u, U32DefaultList.size())) {
    ASSERT_EQ(ExpectValuesU32[i], static_cast<uint32_t>(U32DefaultList[i]));
    ASSERT_EQ(ExpectValuesU32[i], static_cast<int32_t>(I32DefaultList[i]));
  }
}

TEST(ENUM_UTILS, EnumNames) {
  constexpr auto U32CustomList  = EnumNames<U32CustomRange>();
  constexpr auto I32CustomList  = EnumNames<I32CustomRange>();
  constexpr auto U32DefaultList = EnumNames<U32DefaultRange>();
  constexpr auto I32DefaultList = EnumNames<I32DefaultRange>();

  ASSERT_EQ(19, U32CustomList.size());
  ASSERT_EQ(19, I32CustomList.size());
  ASSERT_EQ(7, U32DefaultList.size());
  ASSERT_EQ(7, I32DefaultList.size());

  for (auto i : std::views::iota(0u, U32CustomList.size())) {
    ASSERT_STREQ(ExpectNamesU32[i], std::string(U32CustomList[i]).c_str());
    ASSERT_STREQ(ExpectNamesI32[i], std::string(I32CustomList[i]).c_str());
  }
  for (auto i : std::views::iota(0u, U32DefaultList.size())) {
    ASSERT_STREQ(ExpectNamesU32[i], std::string(U32DefaultList[i]).c_str());
    ASSERT_STREQ(ExpectNamesU32[i], std::string(I32DefaultList[i]).c_str());
  }
}

TEST(ENUM_UTILS, EnumEntries) {
  constexpr auto U32CustomList  = EnumEntries<U32CustomRange>();
  constexpr auto I32CustomList  = EnumEntries<I32CustomRange>();
  constexpr auto U32DefaultList = EnumEntries<U32DefaultRange>();
  constexpr auto I32DefaultList = EnumEntries<I32DefaultRange>();

  ASSERT_EQ(19, U32CustomList.size());
  ASSERT_EQ(19, I32CustomList.size());
  ASSERT_EQ(7, U32DefaultList.size());
  ASSERT_EQ(7, I32DefaultList.size());

  for (auto i : std::views::iota(0u, U32CustomList.size())) {
    ASSERT_EQ(ExpectValuesU32[i], static_cast<uint32_t>(std::get<0>(U32CustomList[i])));
    ASSERT_EQ(ExpectValuesI32[i], static_cast<int32_t>(std::get<0>(I32CustomList[i])));
    ASSERT_STREQ(ExpectNamesU32[i], std::string(std::get<1>(U32CustomList[i])).c_str());
    ASSERT_STREQ(ExpectNamesI32[i], std::string(std::get<1>(I32CustomList[i])).c_str());
  }
  for (auto i : std::views::iota(0u, U32DefaultList.size())) {
    ASSERT_EQ(ExpectValuesU32[i], static_cast<uint32_t>(std::get<0>(U32DefaultList[i])));
    ASSERT_EQ(ExpectValuesU32[i], static_cast<int32_t>(std::get<0>(I32DefaultList[i])));
    ASSERT_STREQ(ExpectNamesU32[i], std::string(std::get<1>(U32DefaultList[i])).c_str());
    ASSERT_STREQ(ExpectNamesU32[i], std::string(std::get<1>(I32DefaultList[i])).c_str());
  }
}

TEST(ENUM_UTILS, EnumName) {
  ASSERT_STREQ("Ox00000001", std::string(EnumName<U32CustomRange>(U32CustomRange::Ox00000001)).c_str());
  ASSERT_STREQ("Ox00000001", std::string(EnumName<I32CustomRange>(I32CustomRange::Ox00000001)).c_str());
  ASSERT_STREQ("Ox80000001", std::string(EnumName<U32CustomRange>(U32CustomRange::Ox80000001)).c_str());
  ASSERT_STREQ("Ox80000001", std::string(EnumName<I32CustomRange>(I32CustomRange::Ox80000001)).c_str());
  ASSERT_STREQ("Ox00000001", std::string(EnumName<U32DefaultRange>(U32DefaultRange::Ox00000001)).c_str());
  ASSERT_STREQ("Ox00000001", std::string(EnumName<I32DefaultRange>(I32DefaultRange::Ox00000001)).c_str());
  ASSERT_STREQ("", std::string(EnumName<U32DefaultRange>(U32DefaultRange::Ox80000001)).c_str());
  ASSERT_STREQ("", std::string(EnumName<I32DefaultRange>(I32DefaultRange::Ox80000001)).c_str());

  ASSERT_STREQ("", std::string(EnumName<U32CustomRange>(static_cast<U32CustomRange>(0xF0000001))).c_str());
  ASSERT_STREQ("", std::string(EnumName<I32CustomRange>(static_cast<I32CustomRange>(0xF0000001))).c_str());
  ASSERT_STREQ("", std::string(EnumName<U32DefaultRange>(static_cast<U32DefaultRange>(0xF0000001))).c_str());
  ASSERT_STREQ("", std::string(EnumName<I32DefaultRange>(static_cast<I32DefaultRange>(0xF0000001))).c_str());
}

TEST(ENUM_UTILS, EnumCount) {
  ASSERT_EQ(19, EnumCount<U32CustomRange>());
  ASSERT_EQ(19, EnumCount<I32CustomRange>());
  ASSERT_EQ(7, EnumCount<U32DefaultRange>());
  ASSERT_EQ(7, EnumCount<I32DefaultRange>());
}

TEST(ENUM_UTILS, EnumCast_String) {
  ASSERT_EQ(std::optional<U32CustomRange>(U32CustomRange::Ox00000001), EnumCast<U32CustomRange>("Ox00000001"));
  ASSERT_EQ(std::optional<I32CustomRange>(I32CustomRange::Ox00000001), EnumCast<I32CustomRange>("Ox00000001"));
  ASSERT_EQ(std::optional<U32CustomRange>(U32CustomRange::Ox80000001), EnumCast<U32CustomRange>("Ox80000001"));
  ASSERT_EQ(std::optional<I32CustomRange>(I32CustomRange::Ox80000001), EnumCast<I32CustomRange>("Ox80000001"));
  ASSERT_EQ(std::optional<U32DefaultRange>(U32DefaultRange::Ox00000001), EnumCast<U32DefaultRange>("Ox00000001"));
  ASSERT_EQ(std::optional<I32DefaultRange>(I32DefaultRange::Ox00000001), EnumCast<I32DefaultRange>("Ox00000001"));
  ASSERT_EQ(std::nullopt, EnumCast<U32DefaultRange>("Ox80000001"));
  ASSERT_EQ(std::nullopt, EnumCast<I32DefaultRange>("Ox80000001"));

  ASSERT_EQ(std::nullopt, EnumCast<U32CustomRange>("OxF0000001"));
  ASSERT_EQ(std::nullopt, EnumCast<I32CustomRange>("OxF0000001"));
  ASSERT_EQ(std::nullopt, EnumCast<U32DefaultRange>("OxF0000001"));
  ASSERT_EQ(std::nullopt, EnumCast<I32DefaultRange>("OxF0000001"));
}

TEST(ENUM_UTILS, EnumCast_Integral) {
  ASSERT_EQ(std::optional<U32CustomRange>(U32CustomRange::Ox00000001), EnumCast<U32CustomRange>(0x00000001));
  ASSERT_EQ(std::optional<I32CustomRange>(I32CustomRange::Ox00000001), EnumCast<I32CustomRange>(0x00000001));
  ASSERT_EQ(std::optional<U32CustomRange>(U32CustomRange::Ox80000001), EnumCast<U32CustomRange>(0x80000001));
  ASSERT_EQ(std::optional<I32CustomRange>(I32CustomRange::Ox80000001), EnumCast<I32CustomRange>(0x80000001));
  ASSERT_EQ(std::optional<U32DefaultRange>(U32DefaultRange::Ox00000001), EnumCast<U32DefaultRange>(0x00000001));
  ASSERT_EQ(std::optional<I32DefaultRange>(I32DefaultRange::Ox00000001), EnumCast<I32DefaultRange>(0x00000001));
  ASSERT_EQ(std::nullopt, EnumCast<U32DefaultRange>(0x80000001));
  ASSERT_EQ(std::nullopt, EnumCast<I32DefaultRange>(0x80000001));

  ASSERT_EQ(std::nullopt, EnumCast<U32CustomRange>(0xF0000001));
  ASSERT_EQ(std::nullopt, EnumCast<I32CustomRange>(0xF0000001));
  ASSERT_EQ(std::nullopt, EnumCast<U32DefaultRange>(0xF0000001));
  ASSERT_EQ(std::nullopt, EnumCast<I32DefaultRange>(0xF0000001));
}

TEST(ENUM_UTILS, EnumContains_String) {
  ASSERT_TRUE(EnumContains<U32CustomRange>("Ox00000001"));
  ASSERT_TRUE(EnumContains<I32CustomRange>("Ox00000001"));
  ASSERT_TRUE(EnumContains<U32CustomRange>("Ox80000001"));
  ASSERT_TRUE(EnumContains<I32CustomRange>("Ox80000001"));
  ASSERT_TRUE(EnumContains<U32DefaultRange>("Ox00000001"));
  ASSERT_TRUE(EnumContains<I32DefaultRange>("Ox00000001"));
  ASSERT_FALSE(EnumContains<U32DefaultRange>("Ox80000001"));
  ASSERT_FALSE(EnumContains<I32DefaultRange>("Ox80000001"));

  ASSERT_FALSE(EnumContains<U32CustomRange>("OxF0000001"));
  ASSERT_FALSE(EnumContains<I32CustomRange>("OxF0000001"));
  ASSERT_FALSE(EnumContains<U32DefaultRange>("OxF0000001"));
  ASSERT_FALSE(EnumContains<I32DefaultRange>("OxF0000001"));
}

TEST(ENUM_UTILS, EnumContains_Integral) {
  ASSERT_TRUE(EnumContains<U32CustomRange>(0x00000001));
  ASSERT_TRUE(EnumContains<I32CustomRange>(0x00000001));
  ASSERT_TRUE(EnumContains<U32CustomRange>(0x80000001));
  ASSERT_TRUE(EnumContains<I32CustomRange>(0x80000001));
  ASSERT_TRUE(EnumContains<U32DefaultRange>(0x00000001));
  ASSERT_TRUE(EnumContains<I32DefaultRange>(0x00000001));
  ASSERT_FALSE(EnumContains<U32DefaultRange>(0x80000001));
  ASSERT_FALSE(EnumContains<I32DefaultRange>(0x80000001));

  ASSERT_FALSE(EnumContains<U32CustomRange>(0xF0000001));
  ASSERT_FALSE(EnumContains<I32CustomRange>(0xF0000001));
  ASSERT_FALSE(EnumContains<U32DefaultRange>(0xF0000001));
  ASSERT_FALSE(EnumContains<I32DefaultRange>(0xF0000001));
}
