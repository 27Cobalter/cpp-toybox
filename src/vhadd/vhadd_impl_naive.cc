#include "vhadd.h"

#include <algorithm>
#include <cassert>
#include <ranges>

constexpr VHAdd::Method VMN = VHAdd::Method::Naive;
template <>
std::span<uint16_t> VHAdd::CalcV_Impl<VMN>(uint16_t* src, int32_t size, int32_t offset_x,
                                           int32_t offset_y, int32_t horizontal,
                                           int32_t vertical) {
  assert(width_ * height_ == size);
  assert((offset_x + horizontal) <= width_);
  assert((offset_y + vertical) <= height_);

  std::span<int32_t> acc = std::span<int32_t>(vaptr_ + offset_x, horizontal);
  result_slice_[0]        = std::span<uint16_t>(vdptr_ + offset_x, horizontal);
  std::ranges::fill(acc, 0);

  for (auto j : std::views::iota(offset_y, offset_y + vertical)) {
    uint16_t* sptrj = src + width_ * j;
    for (auto i : std::views::iota(offset_x, offset_x + horizontal)) {
      vaptr_[i] += sptrj[i];
    }
  }

  float r = 1.0f / vertical + std::numeric_limits<float>::epsilon();
  for (auto i : std::views::iota(offset_x, offset_x + horizontal)) {
    float v   = static_cast<float>(vaptr_[i]);
    uint16_t vr = static_cast<uint16_t>(v * r);
    vdptr_[i] = vr;
  }
  return result_slice_[0];
}
template <>
std::span<uint16_t> VHAdd::CalcH_Impl<VMN>(uint16_t* src, int32_t size, int32_t offset_x,
                                           int32_t offset_y, int32_t horizontal,
                                           int32_t vertical) {
  assert(width_ * height_ == size);
  assert((offset_x + horizontal) <= width_);
  assert((offset_y + vertical) <= height_);

  result_slice_[1]        = std::span<uint16_t>(hdptr_ + offset_y, vertical);

  float r = 1.0 / horizontal + std::numeric_limits<float>::epsilon();
  for (auto j : std::views::iota(offset_y, offset_y + vertical)) {
    uint16_t* sptrj = src + width_ * j;
    int32_t acc     = 0;
    for (auto i : std::views::iota(offset_x, offset_x + horizontal)) {
      acc += sptrj[i];
    }
    // hdptr_[j] = static_cast<uint16_t>(haptr_[j] / static_cast<int32_t>(horizontal));
    hdptr_[j] = static_cast<uint16_t>(static_cast<float>(acc) * r);
  }

  return result_slice_[1];
}
template <>
std::array<std::span<uint16_t>, 2> VHAdd::CalcVH_Impl<VMN>(uint16_t* src, int32_t size,
                                                           int32_t offset_x, int32_t offset_y,
                                                           int32_t horizontal,
                                                           int32_t vertical) {
  assert(width_ * height_ == size);
  assert((offset_x + horizontal) <= width_);
  assert((offset_y + vertical) <= height_);

  std::span<int32_t> acc_slice = std::span<int32_t>(vaptr_ + offset_x, horizontal);
  result_slice_[0]        = std::span<uint16_t>(vdptr_ + offset_x, horizontal);
  result_slice_[1]        = std::span<uint16_t>(hdptr_ + offset_y, vertical);
  std::ranges::fill(acc_slice, 0);

  float rv = 1.0 / vertical + std::numeric_limits<float>::epsilon();
  float rh = 1.0 / horizontal + std::numeric_limits<float>::epsilon();
  for (auto j : std::views::iota(offset_y, offset_y + vertical)) {
    uint16_t* sptrj = src + width_ * j;
    int32_t acc     = 0;
    for (auto i : std::views::iota(offset_x, offset_x + horizontal)) {
      vaptr_[i] += sptrj[i];
      acc += sptrj[i];
    }
    hdptr_[j] = static_cast<uint16_t>(static_cast<float>(acc) * rh);
  }
  for (auto i : std::views::iota(offset_x, offset_x + horizontal)) {
    vdptr_[i] = static_cast<uint16_t>(static_cast<float>(vaptr_[i]) * rv);
  }
  return result_slice_;
}
