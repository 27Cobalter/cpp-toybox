#include "binning.h"

#include <bit>
#include <cassert>
#include <print>
#include <ranges>

#include <immintrin.h>
#include <opencv4/opencv2/core/core.hpp>

template <>
void Binning<Impl::Avx512UnrollX>::Execute(const cv::Mat& src, cv::Mat& dst, uint32_t binning_x,
                                   uint32_t binning_y) {
  Execute_Impl(binning_x, binning_y, src, dst);
}

template <>
template <>
void Binning<Impl::Avx512UnrollX>::Execute_Impl<1, 1>(const cv::Mat& src, cv::Mat& dst) {
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
template <uint32_t BINNING_X, uint32_t BINNING_Y>
void Binning<Impl::Avx512UnrollX>::Execute_Impl(const cv::Mat& src, cv::Mat& dst) {
  static_assert(std::has_single_bit(BINNING_X));
  static_assert(std::has_single_bit(BINNING_Y));

  constexpr int32_t shift_x = std::bit_width(BINNING_X) - 1;
  constexpr int32_t shift_y = std::bit_width(BINNING_Y) - 1;

  assert(src.cols / BINNING_X == dst.cols);
  assert(src.rows / BINNING_Y == dst.rows);
  assert(src.type() == CV_16UC1);
  assert(src.type() == CV_16UC1);

  __m512i idx;
  if constexpr (BINNING_X == 2) {
    idx = _mm512_set_epi16(32 | 30, 32 | 28, 32 | 26, 32 | 24, 32 | 22, 32 | 20, 32 | 18,
                           32 | 16, 32 | 14, 32 | 12, 32 | 10, 32 | 8, 32 | 6, 32 | 4, 32 | 2,
                           32 | 0, 30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10, 8, 6, 4, 2, 0);
  } else if constexpr (BINNING_X == 4) {
    idx = _mm512_set_epi16(32 | 28, 32 | 24, 32 | 20, 32 | 16, 32 | 12, 32 | 8, 32 | 4, 32 | 0,
                           28, 24, 20, 16, 12, 8, 4, 0, 32 | 28, 32 | 24, 32 | 20, 32 | 16,
                           32 | 12, 32 | 8, 32 | 4, 32 | 0, 28, 24, 20, 16, 12, 8, 4, 0);
  }
  for (auto y : std::views::iota(0, src.rows) | std::views::stride(BINNING_Y)) {
    uint16_t* dptry = dst.ptr<uint16_t>(y >> shift_y);

    constexpr int32_t stride = 512 / 8 / sizeof(uint16_t);

    constexpr __mmask32 mask2 = 0b01010101010101010101010101010101;
    constexpr __mmask32 mask4 = 0b00010001000100010001000100010001;
    for (auto x : std::views::iota(0, src.cols) | std::views::stride(stride << 1)) {
      const uint16_t* sptryx = src.ptr<uint16_t>(y + 0) + x;
      __m512i y0_0           = _mm512_loadu_si512(sptryx);
      __m512i y0_1           = _mm512_loadu_si512(sptryx + stride);
      __m512i y1_0           = _mm512_loadu_si512(sptryx + src.step1());
      __m512i y1_1           = _mm512_loadu_si512(sptryx + src.step1() + stride);
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

      if constexpr (BINNING_Y == 4) {
        __m512i y2_0  = _mm512_loadu_si512(sptryx + src.step1() * 2);
        __m512i y2_1  = _mm512_loadu_si512(sptryx + src.step1() * 2 + stride);
        __m512i y3_0  = _mm512_loadu_si512(sptryx + src.step1() * 3);
        __m512i y3_1  = _mm512_loadu_si512(sptryx + src.step1() * 3 + stride);
        __m512i y2_0s = _mm512_srli_epi64(y2_0, 16);
        __m512i y2_1s = _mm512_srli_epi64(y2_1, 16);
        __m512i y3_0s = _mm512_srli_epi64(y3_0, 16);
        __m512i y3_1s = _mm512_srli_epi64(y3_1, 16);
        y2_0          = _mm512_adds_epu16(y2_0, y2_0s);
        y2_1          = _mm512_adds_epu16(y2_1, y2_1s);
        y3_0          = _mm512_adds_epu16(y3_0, y3_0s);
        y3_1          = _mm512_adds_epu16(y3_1, y3_1s);
        y2_0          = _mm512_maskz_adds_epu16(mask2, y2_0, y3_0);
        y2_1          = _mm512_maskz_adds_epu16(mask2, y2_1, y3_1);
        y0_0          = _mm512_maskz_adds_epu16(mask2, y0_0, y2_0);
        y0_1          = _mm512_maskz_adds_epu16(mask2, y0_1, y2_1);
      }

      if constexpr (BINNING_X == 2) {
        y0_0 = _mm512_permutex2var_epi16(y0_0, idx, y0_1);
        _mm512_storeu_si512(dptry + (x >> shift_x), y0_0);
      } else if (BINNING_X == 4) {
        y0_0 = _mm512_maskz_adds_epu16(mask4, y0_0, _mm512_srli_epi64(y0_0, 32));
        y0_1 = _mm512_maskz_adds_epu16(mask4, y0_1, _mm512_srli_epi64(y0_1, 32));
        y0_0 = _mm512_permutex2var_epi16(y0_0, idx, y0_1);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(dptry + (x >> shift_x)),
                            _mm512_castsi512_si256(y0_0));
      }
    }
  }
}
