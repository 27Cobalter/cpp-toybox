#include "lut.h"

#include <algorithm>

template <>
void LUT::Create_Impl<LUT::Method::naive_lut>(int32_t lut_min, int32_t lut_max) {
  uint32_t* lptr = lut_.get();
  float coeff    = 256.0 / (lut_max - lut_min + 1);
  for (int i = 0; i < range_max_; i++) {
    lptr[i] = std::clamp(static_cast<int32_t>(coeff * (i - lut_min)), 0, 255);
  }
}

template <>
void LUT::Convert_Impl<LUT::Method::naive_lut>(uint16_t* src, uint8_t* dst, int32_t data_size) {
  uint32_t* lptr = lut_.get();
  for (int i = 0; i < data_size; i++) {
    dst[i] = lptr[src[i]];
  }
}

template <>
void LUT::Create_Impl<LUT::Method::naive_calc>(int32_t lut_min, int32_t lut_max) {
  coeff_   = 256.0 / (lut_max - lut_min + 1);
  lut_min_ = lut_min;
  lut_max_ = lut_max;
}

template <>
void LUT::Convert_Impl<LUT::Method::naive_calc>(uint16_t* src, uint8_t* dst,
                                                int32_t data_size) {
  for (int i = 0; i < data_size; i++) {
    dst[i] = std::clamp(static_cast<int32_t>(coeff_ * (src[i] - lut_min_)), 0, 255);
  }
}
