#include "lut.h"

#include <immintrin.h>


template <>
void LUT::Convert_Impl<LUT::Method::avx512f_lut>(uint16_t* src, uint8_t* dst,
                                                 int32_t data_size) {
  constexpr int32_t step      = 512 / 8 / sizeof(uint8_t);
  constexpr int32_t half_step = step >> 1;
  int32_t* lptr               = reinterpret_cast<int32_t*>(lut_.get());

  const __m512i zero_v = _mm512_setzero_si512();
  for (int i = 0; i < data_size; i += step) {
    uint16_t* sptri = src + i;
    uint8_t* dptri  = dst + i;
    __m512i src_v   = _mm512_loadu_epi16(sptri);
    __m512i idx_hi  = _mm512_unpackhi_epi16(src_v, zero_v);
    __m512i idx_lo  = _mm512_unpacklo_epi16(src_v, zero_v);
    __m512i hi      = _mm512_i32gather_epi32(idx_hi, lptr, sizeof(uint32_t));
    __m512i lo      = _mm512_i32gather_epi32(idx_lo, lptr, sizeof(uint32_t));
    __m512i pack    = _mm512_packus_epi16(lo, hi);
    __m256i dst_v   = _mm512_cvtepi16_epi8(pack);
    _mm256_storeu_epi8(dptri, dst_v);

    src_v  = _mm512_loadu_epi16(sptri + half_step);
    idx_hi = _mm512_unpackhi_epi16(src_v, zero_v);
    idx_lo = _mm512_unpacklo_epi16(src_v, zero_v);
    hi     = _mm512_i32gather_epi32(idx_hi, lptr, sizeof(uint32_t));
    lo     = _mm512_i32gather_epi32(idx_lo, lptr, sizeof(uint32_t));
    pack   = _mm512_packus_epi16(lo, hi);
    dst_v  = _mm512_cvtepi16_epi8(pack);
    _mm256_storeu_epi8(dptri + half_step, dst_v);
  }
}

template <> // requires AVX-512f
void LUT::Convert_Impl<LUT::Method::avx512f_calc>(uint16_t* src, uint8_t* dst,
                                                  int32_t data_size) {
  constexpr int32_t step      = 512 / 8 / sizeof(uint8_t);
  constexpr int32_t half_step = step >> 1;

  const __m512 coeff_v      = _mm512_set1_ps(coeff_);
  const __m512i lut_min_v   = _mm512_set1_epi16(lut_min_);
  const __m512i uint8_max_v = _mm512_set1_epi32(255);
  const __m512i zero_v      = _mm512_setzero_si512();

  for (int i = 0; i < data_size; i += step) {
    uint16_t* sptri   = src + i;
    uint8_t* dptri    = dst + i;
    __m512i src_sub_v = _mm512_subs_epu16(_mm512_loadu_epi16(sptri), lut_min_v);
    __m512i srcs_hi   = _mm512_unpackhi_epi16(src_sub_v, zero_v);
    __m512i srcs_lo   = _mm512_unpacklo_epi16(src_sub_v, zero_v);
    __m512i val_hi    = _mm512_cvtps_epi32(_mm512_mul_ps(coeff_v, _mm512_cvtepi32_ps(srcs_hi)));
    __m512i hi        = _mm512_min_epi32(val_hi, uint8_max_v);
    __m512i val_lo    = _mm512_cvtps_epi32(_mm512_mul_ps(coeff_v, _mm512_cvtepi32_ps(srcs_lo)));
    __m512i lo        = _mm512_min_epi32(val_lo, uint8_max_v);
    __m512i pack      = _mm512_packus_epi16(lo, hi);
    __m256i dst_v     = _mm512_cvtepi16_epi8(pack);
    _mm256_storeu_epi8(dptri, dst_v);

    src_sub_v = _mm512_subs_epu16(_mm512_loadu_epi16(sptri + half_step), lut_min_v);
    srcs_hi   = _mm512_unpackhi_epi16(src_sub_v, zero_v);
    srcs_lo   = _mm512_unpacklo_epi16(src_sub_v, zero_v);
    val_hi    = _mm512_cvtps_epi32(_mm512_mul_ps(coeff_v, _mm512_cvtepi32_ps(srcs_hi)));
    hi        = _mm512_min_epi32(val_hi, uint8_max_v);
    val_lo    = _mm512_cvtps_epi32(_mm512_mul_ps(coeff_v, _mm512_cvtepi32_ps(srcs_lo)));
    lo        = _mm512_min_epi32(val_lo, uint8_max_v);
    pack      = _mm512_packus_epi16(lo, hi);
    dst_v     = _mm512_cvtepi16_epi8(pack);
    _mm256_storeu_epi8(dptri + half_step, dst_v);
  }
}

