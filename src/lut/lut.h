#pragma once

#include <cstdint>
#include <memory>

class LUT {
private:
public: // for test
  std::shared_ptr<uint32_t[]> lut_ = nullptr;
  int32_t lut_min_                 = 0;
  int32_t lut_max_                 = 0;
  float coeff_                     = 0;
  int32_t range_max_;

public:
  enum class Method {
    naive_lut,
    naive_calc,
    avx2_lut,
    avx2_calc,
    avx2_calc_intweight_epu16,
    avx2_calc_intweight_epi32,
    avx512f_lut,
    avx512f_calc,
    avx512vbmi_lut,
    avx512vbmi_calc,
    avx512vbmi_calc_intweight_epu16,
    avx512vbmi_calc_intweight_epi32,
  };

private:
public: // for test
  template<Method m>
  void Create_Impl(int32_t lut_min, int32_t lut_max);
  template<Method m>
  void Convert_Impl(uint16_t* src, uint8_t* dst, int32_t data_size);

  void ImplSelector();
  void (LUT::*Create_AutoImpl)(int32_t, int32_t);
  void (LUT::*Convert_AutoImpl)(uint16_t*, uint8_t*, int32_t);

public:
  LUT(int32_t range_max);

  void Create(int32_t lut_min, int32_t lut_max);
  void Convert(uint16_t* src, uint8_t* dst, int32_t data_size);
};

inline void LUT::Create(int32_t lut_min, int32_t lut_max) {
  (this->*Create_AutoImpl)(lut_min, lut_max);
}
inline void LUT::Convert(uint16_t* src, uint8_t* dst, int32_t data_size) {
  (this->*Convert_AutoImpl)(src, dst, data_size);
}
