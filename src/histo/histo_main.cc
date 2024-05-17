#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <concepts>
#include <format>
#include <iostream>
#include <new>
#include <random>
#include <ranges>
#include <valarray>

#include <histo.h>

template <typename T, typename U>
  requires std::convertible_to<T, double> && std::convertible_to<U, double>
double CalcMse(std::span<T> t, std::span<U> u) {
  assert(t.size() == u.size());
  double diff = 0;
  for (int i = 0; i < t.size(); i++) {
    diff += std::pow(t[i] - u[i], 2);

#ifndef NDEBUG
    std::cout << std::format("{:3}: {:6}, {:6}, {}", i, t[i], u[i], t[i] - u[i]) << std::endl;
#endif
  }
  return diff;
}

auto main() -> int {
  constexpr int32_t ALIGN_SIZE = 64;
  constexpr int32_t RANGE_MAX  = 0xFFFF;
  constexpr int32_t RANGE_SIZE = RANGE_MAX + 1;

  constexpr int32_t LOOP_COUNT = 1000;

  std::valarray<int32_t> resolution_list = {1024, 2048};

  decltype(std::chrono::high_resolution_clock::now()) start, end;
  std::random_device seed;
  std::mt19937 engine(seed());
  std::normal_distribution<> norm_dist(RANGE_SIZE >> 1, RANGE_SIZE / 10);

  auto src_ptr = std::shared_ptr<uint16_t[]>(new (
      std::align_val_t(ALIGN_SIZE)) uint16_t[resolution_list.max() * resolution_list.max()]);
  auto ref_ptr =
      std::shared_ptr<int32_t[]>(new (std::align_val_t(ALIGN_SIZE)) int32_t[RANGE_SIZE]);
  std::span<int32_t> ref(ref_ptr.get(), RANGE_SIZE);

  for (auto resolution : resolution_list) {
    std::cout << std::format("data_size: {1}x{1}", resolution, resolution) << std::endl;
    const int32_t data_size = resolution * resolution;
    std::span<uint16_t> src(src_ptr.get(), data_size);

    for (auto& elem : src) {
      elem = static_cast<int32_t>(norm_dist(engine)) & RANGE_MAX;
    }
    std::ranges::fill(ref, 0);
    for (auto& elem : src) {
      ref[elem]++;
    }

    MyHisto myhisto(RANGE_SIZE);
    start = std::chrono::high_resolution_clock::now();
    for (auto loop_i : std::views::iota(0, LOOP_COUNT)) {
      myhisto.Create_Impl<MyHisto::Method::Naive>(src.data(), src.size());
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << std::endl;
    std::cout << CalcMse(myhisto.histo_, ref) << std::endl;

    start = std::chrono::high_resolution_clock::now();
    for (auto loop_i : std::views::iota(0, LOOP_COUNT)) {
      myhisto.Create_Impl<MyHisto::Method::AVX512VPOPCNTDQ>(src.data(), src.size());
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << std::endl;
    std::cout << CalcMse(myhisto.histo_, ref) << std::endl;
  }

  return 0;
}
