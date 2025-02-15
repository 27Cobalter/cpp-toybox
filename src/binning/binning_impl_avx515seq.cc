#include "binning.h"

#include <bit>
#include <cassert>
#include <print>
#include <ranges>

#include <immintrin.h>
#include <opencv4/opencv2/core/core.hpp>

template void Binning<Impl::Avx512Seq>::Execute<2, 2>(const cv::Mat&, cv::Mat&);
template void Binning<Impl::Avx512Seq>::Execute<4, 4>(const cv::Mat&, cv::Mat&);

inline void Print(__m512i vec) {
  std::vector<uint16_t> a(32);
  _mm512_storeu_si512(a.data(), vec);
  std::print("[");
  for (auto elem : a) std::print("{:5d},", elem);
  std::println("]");
}

template <>
template <>
void Binning<Impl::Avx512Seq>::Execute<1, 1>(const cv::Mat& src, cv::Mat& dst) {
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
void Binning<Impl::Avx512Seq>::Execute(const cv::Mat& src, cv::Mat& dst) {
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
  for (auto y : std::views::iota(0, src.rows) | std::views::stride(BINNING_Y)) {
    uint16_t* dptry = dst.ptr<uint16_t>(y >> shift_y);
    std::memset(dptry, 0, dst.step);
    int32_t y_b = 0;
    for (; y_b < BINNING_Y; y_b++) {
      const uint16_t* sptry = src.ptr<uint16_t>(y + y_b);
      for (auto x : std::views::iota(0, src.cols) | std::views::stride(stride)) {
        __m512i sv = _mm512_loadu_si512(sptry + x);
        if constexpr (BINNING_X == 2) {
          __m256i dv = _mm256_loadu_si256(reinterpret_cast<__m256i*>(dptry + (x >> shift_x)));
          sv         = _mm512_maskz_adds_epu16(mask2, sv, _mm512_srli_epi64(sv, 16));
          dv         = _mm256_adds_epu16(dv, _mm512_cvtepi32_epi16(sv));
          _mm256_storeu_si256(reinterpret_cast<__m256i*>(dptry + (x >> shift_x)), dv);
        } else if constexpr (BINNING_X == 4) {
          __m128i dv = _mm_loadu_si128(reinterpret_cast<__m128i*>(dptry + (x >> shift_x)));
          sv         = _mm512_maskz_adds_epu16(mask2, sv, _mm512_srli_epi64(sv, 16));
          sv         = _mm512_maskz_adds_epu16(mask4, sv, _mm512_srli_epi64(sv, 32));
          dv         = _mm_adds_epu16(dv, _mm512_cvtepi64_epi16(sv));
          _mm_storeu_si128(reinterpret_cast<__m128i*>(dptry + (x >> shift_x)), dv);
        }
      }
    }
  }
}
