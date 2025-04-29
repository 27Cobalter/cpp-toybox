#include "binning.h"

#include <bit>
#include <cassert>
#include <print>
#include <ranges>

#include <opencv2/core/core.hpp>

template <>
void Binning<Impl::Naive>::Execute(const cv::Mat& src, cv::Mat& dst, uint32_t binning_x,
                                   uint32_t binning_y) {
  Execute_Impl(binning_x, binning_y, src, dst);
}

template <>
template <>
void Binning<Impl::Naive>::Execute_Impl<1, 1>(const cv::Mat& src, cv::Mat& dst) {
  assert(src.cols == dst.cols);
  assert(src.rows == dst.rows);
  assert(src.type() == CV_16UC1);
  assert(src.type() == CV_16UC1);
  std::memcpy(dst.ptr<uint16_t>(), src.ptr<uint16_t>(), src.total() * sizeof(uint16_t));
}

template <>
template <uint32_t BINNING_X, uint32_t BINNING_Y>
void Binning<Impl::Naive>::Execute_Impl(const cv::Mat& src, cv::Mat& dst) {
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
    for (auto x : std::views::iota(0, src.cols) | std::views::stride(BINNING_X)) {
      uint32_t temp = 0;
      for (auto y_b : std::views::iota(0u, BINNING_Y)) {
        const uint16_t* sptryx = src.ptr<uint16_t>(y + y_b) + x;
        for (auto x_b : std::views::iota(0u, BINNING_X)) {
          temp += sptryx[x_b];
        }
      }
      dptry[x >> shift_x] = std::min<uint32_t>(temp, std::numeric_limits<uint16_t>::max());
    }
  }
}
