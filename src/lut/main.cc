#include <iostream>
#include <concepts>
#include <format>
#include <limits>
#include <new>
#include <valarray>
#include <chrono>
#include <ranges>
#include <span>

#include "lut.h"

template <typename T, typename U>
  requires std::convertible_to<T, float> && std::convertible_to<U, float>
float CalcDiff(T* t, U* u, int32_t data_size) {
  float diff = 0;
  for (auto i : std::views::iota(0, data_size)) {
    diff += (t[i] - u[i]) * (t[i] - u[i]);
  }
  return diff / data_size;
}

auto main() -> int {
  constexpr int32_t loop_count = 1000;
  std::valarray<int32_t> width_samples{1024};

  decltype(std::chrono::high_resolution_clock::now()) start, end;
  int32_t time_count;
  float diff;

  std::shared_ptr<uint16_t[]> src(new 
                                      uint16_t[width_samples.max() * width_samples.max()]);
  std::shared_ptr<uint8_t[]> dst(new 
                                     uint8_t[width_samples.max() * width_samples.max()]);
  std::shared_ptr<uint8_t[]> ref(new 
                                     uint8_t[width_samples.max() * width_samples.max()]);

  // constexpr int32_t LUT_END = static_cast<int32_t>(std::numeric_limits<uint16_t>::max()) + 1;
  constexpr int32_t LUT_END = 0xFFFF+1;
  LUT lut(LUT_END);

  for (auto width : width_samples) {
    uint16_t* sptr = src.get();
    uint8_t* dptr  = dst.get();
    uint8_t* rptr  = ref.get();
    uint32_t ref_lut[std::numeric_limits<uint16_t>::max() + 1];

    constexpr int32_t lut_min = 0x000;
    constexpr int32_t lut_max = 0x0FF;
    const int32_t data_size   = width * width;

    // create source
    for (auto i : std::views::iota(0, data_size)) {
      sptr[i] = i & std::numeric_limits<uint16_t>::max();
    }

    // create reference
    for (auto i : std::views::iota(0, LUT_END)) {
      ref_lut[i] = std::clamp(static_cast<int32_t>(255.0 / (lut_max - lut_min) * (i - lut_min) + 0.5), 0, 255);
    }
    std::cout << std::endl;
    for (auto i : std::views::iota(0, data_size)) {
      rptr[i] = ref_lut[sptr[i]];
    }

    // naive lut create
    std::ranges::fill(std::span(lut.lut_.get(), LUT_END), 0);
    start = std::chrono::high_resolution_clock::now();
    for (auto current_loop : std::views::iota(0, loop_count)) {
      lut.Create<LUT::Method::naive_lut>(lut_min, lut_max);
    }
    end        = std::chrono::high_resolution_clock::now();
    time_count = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << std::format("time: {}, diff: {}", time_count / loop_count,
                             CalcDiff(lut.lut_.get(), ref_lut, LUT_END))
              << std::endl;

    // naive lut convert
    std::ranges::fill(std::span(dptr, data_size), 0);
    start = std::chrono::high_resolution_clock::now();
    for (auto current_loop : std::views::iota(0, loop_count)) {
      lut.Convert<LUT::Method::naive_lut>(sptr, dptr, data_size);
    }
    end        = std::chrono::high_resolution_clock::now();
    time_count = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << std::format("time: {}, diff: {}", time_count / loop_count,
                             CalcDiff(dptr, rptr, data_size))
              << std::endl;

    // avx2 lut create
    std::ranges::fill(std::span(lut.lut_.get(), LUT_END), 0);
    start = std::chrono::high_resolution_clock::now();
    for (auto current_loop : std::views::iota(0, loop_count)) {
      lut.Create<LUT::Method::avx2_lut>(lut_min, lut_max);
    }
    end        = std::chrono::high_resolution_clock::now();
    time_count = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << std::format("time: {}, diff: {}", time_count / loop_count,
                             CalcDiff(lut.lut_.get(), ref_lut, LUT_END))
              << std::endl;

    // avx2 lut convert
    std::ranges::fill(std::span(dptr, data_size), 0);
    start = std::chrono::high_resolution_clock::now();
    for (auto current_loop : std::views::iota(0, loop_count)) {
      lut.Convert<LUT::Method::avx2_lut>(sptr, dptr, data_size);
    }
    end        = std::chrono::high_resolution_clock::now();
    time_count = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << std::format("time: {}, diff: {}", time_count / loop_count,
                             CalcDiff(dptr, rptr, data_size))
              << std::endl;

    // avx512 lut convert
    std::ranges::fill(std::span(dptr, data_size), 0);
    start = std::chrono::high_resolution_clock::now();
    for (auto current_loop : std::views::iota(0, loop_count)) {
      lut.Convert<LUT::Method::avx512_lut>(sptr, dptr, data_size);
    }
    end        = std::chrono::high_resolution_clock::now();
    time_count = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << std::format("time: {}, diff: {}", time_count / loop_count,
                             CalcDiff(dptr, rptr, data_size))
              << std::endl;

    // avx512 lut convert
    std::ranges::fill(std::span(dptr, data_size), 0);
    start = std::chrono::high_resolution_clock::now();
    for (auto current_loop : std::views::iota(0, loop_count)) {
      lut.Convert<LUT::Method::avx512_lut_permute>(sptr, dptr, data_size);
    }
    end        = std::chrono::high_resolution_clock::now();
    time_count = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << std::format("time: {}, diff: {}", time_count / loop_count,
                             CalcDiff(dptr, rptr, data_size))
              << std::endl;
  }

  return 0;
}
