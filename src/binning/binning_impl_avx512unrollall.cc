#include "binning.h"

#include <bit>
#include <cassert>
#include <print>
#include <ranges>

#include <immintrin.h>
#include <opencv4/opencv2/core/core.hpp>

inline void Print(__m512i vec) {
  std::vector<uint16_t> a(32);
  _mm512_storeu_si512(a.data(), vec);
  std::print("[");
  for (auto elem : a) std::print("{:5d},", elem);
  std::println("]");
}

template <>
template <>
void Binning<Impl::Avx512UnrollAll>::Execute_Impl<1, 1>(const cv::Mat& src, cv::Mat& dst) {
  assert(src.cols / BINNING_X == dst.cols);
  assert(src.rows / BINNING_Y == dst.rows);
  assert(src.type() == CV_16UC1);
  assert(src.type() == CV_16UC1);
  for (auto y : std::views::iota(0, src.rows)) {
    for (auto x :
         std::views::iota(0, src.cols) | std::views::stride(512 / 8 / sizeof(uint16_t))) {
      _mm512_storeu_si512(dst.ptr<uint16_t>(y) + x,
                          _mm512_loadu_si512(src.ptr<uint16_t>(y) + x));
    }
  }
}

template <>
template <>
void Binning<Impl::Avx512UnrollAll>::Execute_Impl<2, 2>(const cv::Mat& src, cv::Mat& dst) {
  constexpr uint32_t BINNING_X = 2;
  constexpr uint32_t BINNING_Y = 2;
  static_assert(std::has_single_bit(BINNING_X));
  static_assert(std::has_single_bit(BINNING_Y));

  constexpr int32_t shift_x = std::bit_width(BINNING_X) - 1;
  constexpr int32_t shift_y = std::bit_width(BINNING_Y) - 1;

  assert(src.cols / BINNING_X == dst.cols);
  assert(src.rows / BINNING_Y == dst.rows);
  assert(src.type() == CV_16UC1);
  assert(src.type() == CV_16UC1);

  const int32_t src_step1 = src.step1();

  const __m512i idx =
      _mm512_set_epi16(32 | 30, 32 | 28, 32 | 26, 32 | 24, 32 | 22, 32 | 20, 32 | 18, 32 | 16,
                       32 | 14, 32 | 12, 32 | 10, 32 | 8, 32 | 6, 32 | 4, 32 | 2, 32 | 0, 30,
                       28, 26, 24, 22, 20, 18, 16, 14, 12, 10, 8, 6, 4, 2, 0);
  for (auto y : std::views::iota(0, src.rows) | std::views::stride(BINNING_Y)) {
    uint16_t* dptry = dst.ptr<uint16_t>(y >> shift_y);

    constexpr int32_t stride = 512 / 8 / sizeof(uint16_t);

    constexpr __mmask32 mask2 = 0b01010101010101010101010101010101;
    for (auto x : std::views::iota(0, src.cols) | std::views::stride(stride << 1)) {
      const uint16_t* sptryx = src.ptr<uint16_t>(y + 0) + x;
      __m512i y0_0           = _mm512_loadu_si512(sptryx);
      __m512i y0_1           = _mm512_loadu_si512(sptryx + stride);
      __m512i y1_0           = _mm512_loadu_si512(sptryx + src_step1);
      __m512i y1_1           = _mm512_loadu_si512(sptryx + src_step1 + stride);
      __m512i y0_0s          = _mm512_srli_epi64(y0_0, 16);
      __m512i y0_1s          = _mm512_srli_epi64(y0_1, 16);
      __m512i y1_0s          = _mm512_srli_epi64(y1_0, 16);
      __m512i y1_1s          = _mm512_srli_epi64(y1_1, 16);
      y0_0                   = _mm512_adds_epu16(y0_0, y0_0s);
      y0_1                   = _mm512_adds_epu16(y0_1, y0_1s);
      y1_0                   = _mm512_adds_epu16(y1_0, y1_0s);
      y1_1                   = _mm512_adds_epu16(y1_1, y1_1s);
      y0_0                   = _mm512_maskz_adds_epu16(mask2, y0_0, y1_0);
      y0_1                   = _mm512_maskz_adds_epu16(mask2, y0_1, y1_1);

      y0_0 = _mm512_permutex2var_epi16(y0_0, idx, y0_1); // vpermi2w vpermt2w 7/2
      _mm512_storeu_si512(dptry + (x >> shift_x), y0_0);
    }
  }
}

template <>
template <>
void Binning<Impl::Avx512UnrollAll>::Execute_Impl<4, 4>(const cv::Mat& src, cv::Mat& dst) {
  constexpr uint32_t BINNING_X = 4;
  constexpr uint32_t BINNING_Y = 4;
  static_assert(std::has_single_bit(BINNING_X));
  static_assert(std::has_single_bit(BINNING_Y));

  constexpr int32_t shift_x = std::bit_width(BINNING_X) - 1;
  constexpr int32_t shift_y = std::bit_width(BINNING_Y) - 1;

  assert(src.cols / BINNING_X == dst.cols);
  assert(src.rows / BINNING_Y == dst.rows);
  assert(src.type() == CV_16UC1);
  assert(src.type() == CV_16UC1);

  constexpr int32_t stride1 = 512 / 8 / sizeof(uint16_t);
  constexpr int32_t stride2 = 512 / 8 / sizeof(uint16_t) * 2;
  constexpr int32_t stride3 = 512 / 8 / sizeof(uint16_t) * 3;
  const int32_t src_step1   = src.step1();
  const int32_t src_step2   = src.step1() * 2;
  const int32_t src_step3   = src.step1() * 3;

  const __m512i idx =
      _mm512_set_epi16(32 | 30, 32 | 28, 32 | 22, 32 | 20, 32 | 14, 32 | 12, 32 | 6, 32 | 4,
                       32 | 26, 32 | 24, 32 | 18, 32 | 16, 32 | 10, 32 | 8, 32 | 2, 32 | 0, 30,
                       28, 22, 20, 14, 12, 6, 4, 26, 24, 18, 16, 10, 8, 2, 0);
  for (auto y : std::views::iota(0, src.rows) | std::views::stride(BINNING_Y)) {
    uint16_t* dptry = dst.ptr<uint16_t>(y >> shift_y);

    constexpr __mmask32 mask2 = 0b01010101010101010101010101010101;
    constexpr __mmask32 mask4 = 0b00010001000100010001000100010001;
    for (auto x : std::views::iota(0, src.cols) | std::views::stride(stride1 << 2)) {
      const uint16_t* sptryx = src.ptr<uint16_t>(y + 0) + x;
      __m512i y0_0           = _mm512_loadu_si512(sptryx);
      __m512i y0_1           = _mm512_loadu_si512(sptryx + stride1);
      __m512i y0_2           = _mm512_loadu_si512(sptryx + stride2);
      __m512i y0_3           = _mm512_loadu_si512(sptryx + stride3);
      __m512i y1_0           = _mm512_loadu_si512(sptryx + src_step1);
      __m512i y1_1           = _mm512_loadu_si512(sptryx + src_step1 + stride1);
      __m512i y1_2           = _mm512_loadu_si512(sptryx + src_step1 + stride2);
      __m512i y1_3           = _mm512_loadu_si512(sptryx + src_step1 + stride3);

      __m512i y0_0s = _mm512_srli_epi64(y0_0, 16);
      __m512i y0_1s = _mm512_srli_epi64(y0_1, 16);
      __m512i y1_0s = _mm512_srli_epi64(y1_0, 16);
      __m512i y1_1s = _mm512_srli_epi64(y1_1, 16);
      y0_0          = _mm512_adds_epu16(y0_0, y0_0s);
      y0_1          = _mm512_adds_epu16(y0_1, y0_1s);
      y1_0          = _mm512_adds_epu16(y1_0, y1_0s);
      y1_1          = _mm512_adds_epu16(y1_1, y1_1s);
      __m512i y0_0t = _mm512_adds_epu16(y0_0, y1_0);
      __m512i y0_1t = _mm512_adds_epu16(y0_1, y1_1);

      y0_0s         = _mm512_srli_epi64(y0_2, 16);
      y0_1s         = _mm512_srli_epi64(y0_3, 16);
      y1_0s         = _mm512_srli_epi64(y1_2, 16);
      y1_1s         = _mm512_srli_epi64(y1_3, 16);
      y0_2          = _mm512_adds_epu16(y0_2, y0_0s);
      y0_3          = _mm512_adds_epu16(y0_3, y0_1s);
      y1_2          = _mm512_adds_epu16(y1_2, y1_0s);
      y1_3          = _mm512_adds_epu16(y1_3, y1_1s);
      __m512i y0_2t = _mm512_adds_epu16(y0_2, y1_2);
      __m512i y0_3t = _mm512_adds_epu16(y0_3, y1_3);

      y0_0 = _mm512_loadu_si512(sptryx + src_step2);
      y0_1 = _mm512_loadu_si512(sptryx + src_step2 + stride1);
      y0_2 = _mm512_loadu_si512(sptryx + src_step2 + stride2);
      y0_3 = _mm512_loadu_si512(sptryx + src_step2 + stride3);
      y1_0 = _mm512_loadu_si512(sptryx + src_step3);
      y1_1 = _mm512_loadu_si512(sptryx + src_step3 + stride1);
      y1_2 = _mm512_loadu_si512(sptryx + src_step3 + stride2);
      y1_3 = _mm512_loadu_si512(sptryx + src_step3 + stride3);

      y0_0s = _mm512_srli_epi64(y0_0, 16);
      y0_1s = _mm512_srli_epi64(y0_1, 16);
      y1_0s = _mm512_srli_epi64(y1_0, 16);
      y1_1s = _mm512_srli_epi64(y1_1, 16);
      y0_0  = _mm512_adds_epu16(y0_0, y0_0s);
      y0_1  = _mm512_adds_epu16(y0_1, y0_1s);
      y1_0  = _mm512_adds_epu16(y1_0, y1_0s);
      y1_1  = _mm512_adds_epu16(y1_1, y1_1s);
      y0_0t = _mm512_adds_epu16(y0_0t, _mm512_adds_epu16(y0_0, y1_0));
      y0_1t = _mm512_adds_epu16(y0_1t, _mm512_adds_epu16(y0_1, y1_1));

      y0_0s = _mm512_srli_epi64(y0_2, 16);
      y0_1s = _mm512_srli_epi64(y0_3, 16);
      y1_0s = _mm512_srli_epi64(y1_2, 16);
      y1_1s = _mm512_srli_epi64(y1_3, 16);
      y0_2  = _mm512_adds_epu16(y0_2, y0_0s);
      y0_3  = _mm512_adds_epu16(y0_3, y0_1s);
      y1_2  = _mm512_adds_epu16(y1_2, y1_0s);
      y1_3  = _mm512_adds_epu16(y1_3, y1_1s);
      y0_2t = _mm512_adds_epu16(y0_2t, _mm512_adds_epu16(y0_2, y1_2));
      y0_3t = _mm512_adds_epu16(y0_3t, _mm512_adds_epu16(y0_3, y1_3));

      y0_0t = _mm512_maskz_adds_epu16(mask4, y0_0t, _mm512_srli_epi64(y0_0t, 32));
      y0_1t = _mm512_maskz_adds_epu16(mask4, y0_1t, _mm512_srli_epi64(y0_1t, 32));
      y0_2t = _mm512_maskz_adds_epu16(mask4, y0_2t, _mm512_srli_epi64(y0_2t, 32));
      y0_3t = _mm512_maskz_adds_epu16(mask4, y0_3t, _mm512_srli_epi64(y0_3t, 32));

      y0_0t = _mm512_packus_epi32(y0_0t, y0_1t);
      y0_2t = _mm512_packus_epi32(y0_2t, y0_3t);

      y0_0t = _mm512_permutex2var_epi16(y0_0t, idx, y0_2t); // vpermi2w vpermt2w 7/2
      _mm512_storeu_si512(dptry + (x >> shift_x), y0_0t);
    }
  }
}
