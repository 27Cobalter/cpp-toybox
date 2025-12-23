#pragma GCC target("avx512f,avx512bw,avx512vl")

#include "binning.h"

#include <bit>
#include <cassert>
#include <print>
#include <ranges>

#include <immintrin.h>
#include <opencv2/core/core.hpp>

template<>
void Binning<Impl::Avx512>::Execute(const cv::Mat& src, cv::Mat& dst, uint32_t binning_x, uint32_t binning_y) {
  Execute_Impl(binning_x, binning_y, src, dst);
}

template<>
template<>
void Binning<Impl::Avx512>::Execute_Impl<1, 1>(const cv::Mat& src, cv::Mat& dst) {
  constexpr uint32_t BINNING_X = 1;
  constexpr uint32_t BINNING_Y = 1;
  assert(src.cols == dst.cols);
  assert(src.rows == dst.rows);
  assert(src.type() == CV_16UC1);
  assert(src.type() == CV_16UC1);
  for (auto y : std::views::iota(0, src.rows)) {
    for (auto x : std::views::iota(0, src.cols) | std::views::stride(512 / 8 / sizeof(uint16_t))) {
      _mm512_storeu_si512(dst.ptr<uint16_t>(y) + x, _mm512_loadu_si512(reinterpret_cast<const void*>(src.ptr<uint16_t>(y) + x)));
    }
  }
}

template<>
template<uint32_t BINNING_X, uint32_t BINNING_Y>
void Binning<Impl::Avx512>::Execute_Impl(const cv::Mat& src, cv::Mat& dst) {
  static_assert(std::has_single_bit(BINNING_X));
  static_assert(std::has_single_bit(BINNING_Y));

  constexpr int32_t shift_x = std::bit_width(BINNING_X) - 1;
  constexpr int32_t shift_y = std::bit_width(BINNING_Y) - 1;

  assert(src.cols / BINNING_X == dst.cols);
  assert(src.rows / BINNING_Y == dst.rows);
  assert(src.type() == CV_16UC1);
  assert(src.type() == CV_16UC1);

  for (auto y : std::views::iota(0, src.rows) | std::views::stride(BINNING_Y)) {
    uint16_t* dptry = dst.ptr<uint16_t>(y >> shift_y);

    constexpr int32_t stride = 512 / 8 / sizeof(uint16_t);

    constexpr __mmask32 mask2 = 0b01010101010101010101010101010101;
    constexpr __mmask32 mask4 = 0b00010001000100010001000100010001;
    for (auto x : std::views::iota(0, src.cols) | std::views::stride(stride)) {
      const uint16_t* sptryx = src.ptr<uint16_t>(y + 0) + x;
      __m512i y0             = _mm512_loadu_si512(reinterpret_cast<const void*>(sptryx));
      __m512i y1             = _mm512_loadu_si512(reinterpret_cast<const void*>(sptryx + src.step1()));
      __m512i y0_            = _mm512_srli_epi64(y0, 16);
      __m512i y1_            = _mm512_srli_epi64(y1, 16);
      y0                     = _mm512_adds_epu16(y0, y0_);
      y1                     = _mm512_adds_epu16(y1, y1_);
      y0                     = _mm512_maskz_adds_epu16(mask2, y0, y1);

      if constexpr (BINNING_Y == 4) {
        __m512i y2  = _mm512_loadu_si512(reinterpret_cast<const void*>(sptryx + src.step1() * 2));
        __m512i y3  = _mm512_loadu_si512(reinterpret_cast<const void*>(sptryx + src.step1() * 3));
        __m512i y2_ = _mm512_srli_epi64(y2, 16);
        __m512i y3_ = _mm512_srli_epi64(y3, 16);
        y2          = _mm512_adds_epu16(y2, y2_);
        y3          = _mm512_adds_epu16(y3, y3_);
        y2          = _mm512_maskz_adds_epu16(mask2, y2, y3);
        y0          = _mm512_maskz_adds_epu16(mask2, y0, y2);
      }

      if constexpr (BINNING_X == 2) {
        _mm512_mask_cvtepi32_storeu_epi16(dptry + (x >> shift_x), 0xFFFF, y0);
      } else if (BINNING_X == 4) {
        y0 = _mm512_maskz_adds_epu16(mask4, y0, _mm512_srli_epi64(y0, 32));
        _mm512_mask_cvtepi64_storeu_epi16(dptry + (x >> shift_x), 0xFF, y0);
      }
    }
  }
}
