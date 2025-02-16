#include "binning.h"

#include <bit>
#include <cassert>
#include <print>
#include <ranges>

#include <immintrin.h>
#include <omp.h>
#include <opencv4/opencv2/core/core.hpp>

template <>
void Binning<Impl::Avx512SeqBuffer>::Execute(const cv::Mat& src, cv::Mat& dst, uint32_t binning_x,
                                   uint32_t binning_y) {
  Execute_Impl(binning_x, binning_y, src, dst);
}

template <>
template <>
void Binning<Impl::Avx512SeqBuffer>::Execute_Impl<1, 1>(const cv::Mat& src, cv::Mat& dst) {
  assert(src.cols == dst.cols);
  assert(src.rows == dst.rows);
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
void Binning<Impl::Avx512SeqBuffer>::Execute_Impl(const cv::Mat& src, cv::Mat& dst) {
  static_assert(std::has_single_bit(BINNING_X));
  static_assert(std::has_single_bit(BINNING_Y));

  constexpr int32_t shift_x = std::bit_width(BINNING_X) - 1;
  constexpr int32_t shift_y = std::bit_width(BINNING_Y) - 1;

  assert(src.cols / BINNING_X == dst.cols);
  assert(src.rows / BINNING_Y == dst.rows);
  assert(src.type() == CV_16UC1);
  assert(src.type() == CV_16UC1);

  constexpr __mmask32 mask2 = 0b01010101010101010101010101010101;
  constexpr __mmask32 mask4 = 0b00010001000100010001000100010001;
  constexpr int32_t stride  = 512 / 8 / sizeof(uint16_t);

  cv::Mat buffer = cv::Mat(cv::Size(src.cols, 1), CV_16UC1);
  uint16_t* bptr = buffer.ptr<uint16_t>();
  for (auto y : std::views::iota(0, src.rows) | std::views::stride(BINNING_Y)) {
    int32_t y_b = 0;
    {
      const uint16_t* sptry = src.ptr<uint16_t>(y + y_b);
      for (auto x : std::views::iota(0, src.cols) | std::views::stride(stride)) {
        __m512i sv = _mm512_loadu_si512(sptry + x);
        if constexpr (BINNING_X == 2) {
          sv = _mm512_maskz_adds_epu16(mask2, sv, _mm512_srli_epi64(sv, 16));
        } else if constexpr (BINNING_X == 4) {
          sv = _mm512_maskz_adds_epu16(mask2, sv, _mm512_srli_epi64(sv, 16));
          sv = _mm512_maskz_adds_epu16(mask4, sv, _mm512_srli_epi64(sv, 32));
        }
        _mm512_storeu_si512(bptr + x, sv);
      }
    }
    for (y_b = 1; y_b < BINNING_Y - 1; y_b++) {
      const uint16_t* sptry = src.ptr<uint16_t>(y + y_b);
      for (auto x : std::views::iota(0, src.cols) | std::views::stride(stride)) {
        __m512i sv = _mm512_loadu_si512(sptry + x);
        __m512i bv = _mm512_loadu_si512(bptr + x);
        if constexpr (BINNING_X == 2) {
          sv = _mm512_maskz_adds_epu16(mask2, sv, _mm512_srli_epi64(sv, 16));
        } else if constexpr (BINNING_X == 4) {
          sv = _mm512_maskz_adds_epu16(mask2, sv, _mm512_srli_epi64(sv, 16));
          sv = _mm512_maskz_adds_epu16(mask4, sv, _mm512_srli_epi64(sv, 32));
        }
        bv = _mm512_adds_epu16(bv, sv);
        _mm512_storeu_si512(bptr + x, bv);
      }
    }
    {
      uint16_t* dptry       = dst.ptr<uint16_t>(y >> shift_y);
      const uint16_t* sptry = src.ptr<uint16_t>(y + y_b);
      for (auto x : std::views::iota(0, src.cols) | std::views::stride(stride)) {
        __m512i sv = _mm512_loadu_si512(sptry + x);
        __m512i bv = _mm512_loadu_si512(bptr + x);
        if constexpr (BINNING_X == 2) {
          sv = _mm512_maskz_adds_epu16(mask2, sv, _mm512_srli_epi64(sv, 16));
          bv = _mm512_adds_epu16(bv, sv);
          _mm256_storeu_si256(reinterpret_cast<__m256i*>(dptry + (x >> shift_x)),
                              _mm512_cvtepi32_epi16(bv));
        } else if constexpr (BINNING_X == 4) {
          sv = _mm512_maskz_adds_epu16(mask2, sv, _mm512_srli_epi64(sv, 16));
          sv = _mm512_maskz_adds_epu16(mask4, sv, _mm512_srli_epi64(sv, 32));
          bv = _mm512_adds_epu16(bv, sv);
          _mm_storeu_si128(reinterpret_cast<__m128i*>(dptry + (x >> shift_x)),
                           _mm512_cvtepi64_epi16(bv));
        }
      }
    }
  }
}
