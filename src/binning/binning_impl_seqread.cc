
#include "binning.h"

#include <bit>
#include <cassert>
#include <print>
#include <ranges>

#include <opencv4/opencv2/core/core.hpp>

template <>
void Binning<Impl::SeqRead>::Execute(const cv::Mat& src, cv::Mat& dst, uint32_t binning_x,
                                   uint32_t binning_y) {
  Execute_Impl(binning_x, binning_y, src, dst);
}

template <>
template <>
void Binning<Impl::SeqRead>::Execute_Impl<1, 1>(const cv::Mat& src, cv::Mat& dst) {
  for (auto y : std::views::iota(0, src.rows)) {
    for (auto x : std::views::iota(0, src.cols)) {
      dst.ptr<uint16_t>(y)[x] = src.ptr<uint16_t>(y)[x];
    }
  }
}

template <>
template <uint32_t BINNING_X, uint32_t BINNING_Y>
void Binning<Impl::SeqRead>::Execute_Impl(const cv::Mat& src, cv::Mat& dst) {
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

    {
      const uint16_t* sptry = src.ptr<uint16_t>(y);
      for (auto x : std::views::iota(0, src.cols) | std::views::stride(BINNING_X)) {
        uint16_t* dptryx = dptry + (x >> shift_x);
        *dptryx          = 0;
        for (auto x_b : std::views::iota(0u, BINNING_X)) {
          *dptryx += sptry[x + x_b];
        }
      }
    }
    for (auto y_b : std::views::iota(1u, BINNING_Y - 1)) {
      const uint16_t* sptry = src.ptr<uint16_t>(y + y_b);
      for (auto x : std::views::iota(0, src.cols) | std::views::stride(BINNING_X)) {
        uint16_t* dptryx = dptry + (x >> shift_x);
        for (auto x_b : std::views::iota(0u, BINNING_X)) {
          *dptryx += sptry[x + x_b];
        }
      }
    }
    {
      const uint16_t* sptry = src.ptr<uint16_t>(y + BINNING_Y - 1);
      for (auto x : std::views::iota(0, src.cols) | std::views::stride(BINNING_X)) {
        uint16_t* dptryx = dptry + (x >> shift_x);
        for (auto x_b : std::views::iota(0u, BINNING_X)) {
          *dptryx += sptry[x + x_b];
        }
      }
    }
  }
}
