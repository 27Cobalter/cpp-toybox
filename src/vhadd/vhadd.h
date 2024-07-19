#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <memory>
#include <span>

class VHAdd {
private:
  std::shared_ptr<int32_t[]> v_acc_;
  std::shared_ptr<uint16_t[]> v_dst_;
  std::shared_ptr<uint16_t[]> h_dst_;
  int32_t* vaptr_;
  uint16_t* vdptr_;
  uint16_t* hdptr_;
  // 0: v, 1: h
  std::array<std::span<uint16_t>, 2> result_slice_;

  int32_t width_;
  int32_t height_;

public:
  enum class Method { Naive, AVX2, AVX2_Vertical, AVX512 };

private:
  float CalcRcp(float v);

public: // for test
  template <Method m>
  std::span<uint16_t> CalcV_Impl(uint16_t* src, int32_t size, int32_t offset_x,
                                 int32_t offset_y, int32_t horizontal, int32_t vertical);
  template <Method m>
  std::span<uint16_t> CalcH_Impl(uint16_t* src, int32_t size, int32_t offset_x,
                                 int32_t offset_y, int32_t horizontal, int32_t vertical);
  template <Method m>
  std::array<std::span<uint16_t>, 2> CalcVH_Impl(uint16_t* src, int32_t size, int32_t offset_x,
                                                 int32_t offset_y, int32_t horizontal,
                                                 int32_t vertical);

public:
  VHAdd(const int32_t width, const int32_t height);
};

inline float VHAdd::CalcRcp(float v) {
  // accurate rcp
  float r_tmp = 1.0f / v;
  float e     = 1.0f - r_tmp * v;
  return std::fma<float>(r_tmp, e, r_tmp);
}
