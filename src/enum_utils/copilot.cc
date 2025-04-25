#include <array>
#include <iostream>
#include <optional>
#include <utility>

// 条件判定用のテンプレート構造体
template <auto Value>
struct IsEven {
  static constexpr std::optional<decltype(Value)> value =
      (Value % 16 == 0) ? std::optional<decltype(Value)>(Value) : std::nullopt;
};

// コンパイル時に条件を満たす要素をリストアップする再帰的テンプレート
template <auto Min, auto Max, template <auto> typename Predicate, size_t... Indices>
consteval auto create_filtered_list_impl(std::index_sequence<Indices...>) {
  constexpr std::array values = {(Predicate<Min + Indices>::value.value_or(-1))...};

  // 有効な値のみを抽出（-1は無効値としてフィルタリング）
  constexpr size_t count = ((values[Indices] != -1) + ...);
  std::array<int, count> result{};
  size_t index = 0;
  for (size_t i = 0; i < values.size(); ++i) {
    if (values[i] != -1) {
      result[index++] = values[i];
    }
  }

  return result;
}

// メインの関数
template <auto Min, auto Max, template <auto> typename Predicate>
consteval auto create_filtered_list() {
  return create_filtered_list_impl<Min, Max, Predicate>(
      std::make_index_sequence<Max - Min + 1>{});
}

template <size_t size, std::size_t... sizes>
auto concatenate(const std::array<int32_t, sizes>&... arrays) {
  std::array<int32_t, size> result{};
  size_t index = 0;
  ((std::copy(arrays.begin(), arrays.end(), result.begin() + index), index += arrays.size()),
   ...);
  return result;
}

int main() {
  // コンパイル時に偶数をフィルタリング
  constexpr auto filtered_list0 = create_filtered_list<0 + 1024 * 0, 1023 + 1024 * 0, IsEven>();
  constexpr auto filtered_list1 = create_filtered_list<0 + 1024 * 1, 1024 + 1024 * 1, IsEven>();
  constexpr auto filtered_list2 = create_filtered_list<0 + 1024 * 2, 1024 + 1024 * 2, IsEven>();
  constexpr auto filtered_list3 = create_filtered_list<0 + 1024 * 3, 1024 + 1024 * 3, IsEven>();
  constexpr auto filtered_list4 = create_filtered_list<0 + 1024 * 4, 1024 + 1024 * 4, IsEven>();
  constexpr auto filtered_list5 = create_filtered_list<0 + 1024 * 5, 1024 + 1024 * 5, IsEven>();
  constexpr auto filtered_list6 = create_filtered_list<0 + 1024 * 6, 1024 + 1024 * 6, IsEven>();
  constexpr auto filtered_list7 = create_filtered_list<0 + 1024 * 7, 1024 + 1024 * 7, IsEven>();
  constexpr size_t s1 = filtered_list0.size() + filtered_list1.size() + filtered_list2.size() +
                        filtered_list3.size() + filtered_list4.size() + filtered_list5.size() +
                        filtered_list6.size() + filtered_list7.size();

  auto filtered_list =
      concatenate<s1>(filtered_list0, filtered_list1, filtered_list2, filtered_list3,
                      filtered_list4, filtered_list5, filtered_list6, filtered_list7);
  // constexpr auto filtered_list = create_filtered_list<8192, 8192 * 2 , IsEven>();

  // 実行時確認
  for (const auto& val : filtered_list) {
    std::cout << val << " "; // 出力: 2 4 6 8 10
  }

  return 0;
}
