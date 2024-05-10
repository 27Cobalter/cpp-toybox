#pragma once

#include <memory>

class LUT {
public:
  std::shared_ptr<uint32_t[]> lut_ = nullptr;
  int32_t min_ = 0;
  int32_t coeff_ = 0;
  int32_t range_max_;

public:
  enum class Method { naive_lut, naive_calc, avx2_lut, avx2_calc, avx512_lut, avx512_calc };
  LUT(int32_t range_max);
  template<Method m>
  void Create(int32_t lut_min, int32_t lut_max);
  template<Method m>
  void Convert(uint16_t* src, uint8_t* dst, int32_t data_size);
};
