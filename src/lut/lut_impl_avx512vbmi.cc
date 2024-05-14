#include "lut.h"

#include <cassert>
#include <cmath>

#include <immintrin.h>

template <>
void LUT::Convert_Impl<LUT::Method::avx512vbmi_lut>(uint16_t* src, uint8_t* dst,
                                                    int32_t data_size) {
  constexpr int32_t step      = 512 / 8 / sizeof(uint8_t);
  constexpr int32_t half_step = step >> 1;
  int32_t* lptr               = reinterpret_cast<int32_t*>(lut_.get());

  const __m512i zero_v = _mm512_setzero_si512();
  // const uint8_t permute_index[64] = {
  //     00, 4,  8,  12, 0x40 | 0,  0x40 | 4,  0x40 | 8,  0x40 | 12,
  //     16, 20, 24, 28, 0x40 | 16, 0x40 | 20, 0x40 | 24, 0x40 | 28,
  //     32, 36, 40, 44, 0x40 | 32, 0x40 | 36, 0x40 | 40, 0x40 | 44,
  //     48, 52, 56, 60, 0x40 | 48, 0x40 | 52, 0x40 | 56, 0x40 | 60,
  //     0,  4,  8,  12, 0x40 | 0,  0x40 | 4,  0x40 | 8,  0x40 | 12,
  //     16, 20, 24, 28, 0x40 | 16, 0x40 | 20, 0x40 | 24, 0x40 | 28,
  //     32, 36, 40, 44, 0x40 | 32, 0x40 | 36, 0x40 | 40, 0x40 | 44,
  //     48, 52, 56, 60, 0x40 | 48, 0x40 | 52, 0x40 | 56, 0x40 | 60};
  // const __m512i permute_index_v = _mm512_loadu_si512(permute_index);
  // setr_epi8 not exists
  const __m512i permute_index_v = _mm512_set_epi8(
      60 | 0x40, 56 | 0x40, 52 | 0x40, 48 | 0x40, 60, 56, 52, 48, 44 | 0x40, 40 | 0x40,
      36 | 0x40, 32 | 0x40, 44, 40, 36, 32, 28 | 0x40, 24 | 0x40, 20 | 0x40, 16 | 0x40, 28, 24,
      20, 16, 12 | 0x40, 8 | 0x40, 4 | 0x40, 0 | 0x40, 12, 8, 4, 0, 60 | 0x40, 56 | 0x40,
      52 | 0x40, 48 | 0x40, 60, 56, 52, 48, 44 | 0x40, 40 | 0x40, 36 | 0x40, 32 | 0x40, 44, 40,
      36, 32, 28 | 0x40, 24 | 0x40, 20 | 0x40, 16 | 0x40, 28, 24, 20, 16, 12 | 0x40, 8 | 0x40,
      4 | 0x40, 0 | 0x40, 12, 8, 4, 0);

  for (int i = 0; i < data_size; i += step) {
    uint16_t* sptri = src + i;
    uint8_t* dptri  = dst + i;
    __m512i src_v   = _mm512_loadu_epi16(sptri);
    __m512i idx_hi  = _mm512_unpackhi_epi16(src_v, zero_v);
    __m512i idx_lo  = _mm512_unpacklo_epi16(src_v, zero_v);
    __m512i hi      = _mm512_i32gather_epi32(idx_hi, lptr, sizeof(uint32_t));
    __m512i lo      = _mm512_i32gather_epi32(idx_lo, lptr, sizeof(uint32_t));
    __m512i dst_v1  = _mm512_permutex2var_epi8(lo, permute_index_v, hi);

    src_v          = _mm512_loadu_epi16(sptri + half_step);
    idx_hi         = _mm512_unpackhi_epi16(src_v, zero_v);
    idx_lo         = _mm512_unpacklo_epi16(src_v, zero_v);
    hi             = _mm512_i32gather_epi32(idx_hi, lptr, sizeof(uint32_t));
    lo             = _mm512_i32gather_epi32(idx_lo, lptr, sizeof(uint32_t));
    __m512i dst_v2 = _mm512_permutex2var_epi8(lo, permute_index_v, hi);

    _mm512_storeu_epi8(dptri, _mm512_mask_blend_epi32(0b1111111100000000, dst_v1, dst_v2));
  }
}

template <> // requires AVX-512 VBMI: Cannon Lake or Tager Lake later or Zen4 or Zen4
void LUT::Convert_Impl<LUT::Method::avx512vbmi_calc>(uint16_t* src, uint8_t* dst,
                                                     int32_t data_size) {
  constexpr int32_t step      = 512 / 8 / sizeof(uint8_t);
  constexpr int32_t half_step = step >> 1;

  const __m512 coeff_v      = _mm512_set1_ps(coeff_);
  const __m512i lut_min_v   = _mm512_set1_epi16(lut_min_);
  const __m512i uint8_max_v = _mm512_set1_epi32(255);
  const __m512i zero_v      = _mm512_setzero_si512();
  // const uint8_t permute_index[64] = {
  //     00, 4,  8,  12, 0x40 | 0,  0x40 | 4,  0x40 | 8,  0x40 | 12,
  //     16, 20, 24, 28, 0x40 | 16, 0x40 | 20, 0x40 | 24, 0x40 | 28,
  //     32, 36, 40, 44, 0x40 | 32, 0x40 | 36, 0x40 | 40, 0x40 | 44,
  //     48, 52, 56, 60, 0x40 | 48, 0x40 | 52, 0x40 | 56, 0x40 | 60,
  //     0,  4,  8,  12, 0x40 | 0,  0x40 | 4,  0x40 | 8,  0x40 | 12,
  //     16, 20, 24, 28, 0x40 | 16, 0x40 | 20, 0x40 | 24, 0x40 | 28,
  //     32, 36, 40, 44, 0x40 | 32, 0x40 | 36, 0x40 | 40, 0x40 | 44,
  //     48, 52, 56, 60, 0x40 | 48, 0x40 | 52, 0x40 | 56, 0x40 | 60};
  // const __m512i permute_index_v = _mm512_loadu_si512(permute_index);
  // setr_epi8 not exists
  const __m512i permute_index_v = _mm512_set_epi8(
      60 | 0x40, 56 | 0x40, 52 | 0x40, 48 | 0x40, 60, 56, 52, 48, 44 | 0x40, 40 | 0x40,
      36 | 0x40, 32 | 0x40, 44, 40, 36, 32, 28 | 0x40, 24 | 0x40, 20 | 0x40, 16 | 0x40, 28, 24,
      20, 16, 12 | 0x40, 8 | 0x40, 4 | 0x40, 0 | 0x40, 12, 8, 4, 0, 60 | 0x40, 56 | 0x40,
      52 | 0x40, 48 | 0x40, 60, 56, 52, 48, 44 | 0x40, 40 | 0x40, 36 | 0x40, 32 | 0x40, 44, 40,
      36, 32, 28 | 0x40, 24 | 0x40, 20 | 0x40, 16 | 0x40, 28, 24, 20, 16, 12 | 0x40, 8 | 0x40,
      4 | 0x40, 0 | 0x40, 12, 8, 4, 0);

  for (int i = 0; i < data_size; i += step) {
    uint16_t* sptri   = src + i;
    __m512i src_sub_v = _mm512_subs_epu16(_mm512_loadu_epi16(sptri), lut_min_v);
    __m512i srcs_hi   = _mm512_unpackhi_epi16(src_sub_v, zero_v);
    __m512i srcs_lo   = _mm512_unpacklo_epi16(src_sub_v, zero_v);
    __m512i val_hi    = _mm512_cvtps_epi32(_mm512_mul_ps(coeff_v, _mm512_cvtepi32_ps(srcs_hi)));
    __m512i hi        = _mm512_min_epi32(val_hi, uint8_max_v);
    __m512i val_lo    = _mm512_cvtps_epi32(_mm512_mul_ps(coeff_v, _mm512_cvtepi32_ps(srcs_lo)));
    __m512i lo        = _mm512_min_epi32(val_lo, uint8_max_v);

    __m512i dst_v1 = _mm512_permutex2var_epi8(lo, permute_index_v, hi);

    src_sub_v      = _mm512_subs_epu16(_mm512_loadu_epi16(sptri + half_step), lut_min_v);
    srcs_hi        = _mm512_unpackhi_epi16(src_sub_v, zero_v);
    srcs_lo        = _mm512_unpacklo_epi16(src_sub_v, zero_v);
    val_hi         = _mm512_cvtps_epi32(_mm512_mul_ps(coeff_v, _mm512_cvtepi32_ps(srcs_hi)));
    hi             = _mm512_min_epi32(val_hi, uint8_max_v);
    val_lo         = _mm512_cvtps_epi32(_mm512_mul_ps(coeff_v, _mm512_cvtepi32_ps(srcs_lo)));
    lo             = _mm512_min_epi32(val_lo, uint8_max_v);
    __m512i dst_v2 = _mm512_permutex2var_epi8(lo, permute_index_v, hi);

    _mm512_storeu_epi8(dst + i, _mm512_mask_blend_epi32(0b1111111100000000, dst_v1, dst_v2));
  }
}

template <> // requires AVX-512 VBMI: Cannon Lake or Tager Lake later or Zen4 or Zen4 // WIP
void LUT::Convert_Impl<LUT::Method::avx512vbmi_calc_intweight_epu16>(uint16_t* src,
                                                                     uint8_t* dst,
                                                                     int32_t data_size) {
  constexpr int32_t step      = 512 / 8 / sizeof(uint8_t);
  constexpr int32_t half_step = step >> 1;

  const __m512i coeff_v               = _mm512_set1_epi16(std::ceil(coeff_ * (0x100)));
  const __m512i lut_min_v             = _mm512_set1_epi16(lut_min_);
  const __m512i uint8_max_div_coeff_v = _mm512_set1_epi16(255.0 / coeff_);
  const uint8_t permute_index[64]     = {
      0,         2,         4,         6,         8,         10,        12,        14,
      16,        18,        20,        22,        24,        26,        28,        30,
      32,        34,        36,        38,        40,        42,        44,        46,
      48,        50,        52,        54,        56,        58,        60,        62,
      0x40 | 0,  0x40 | 2,  0x40 | 4,  0x40 | 6,  0x40 | 8,  0x40 | 10, 0x40 | 12, 0x40 | 14,
      0x40 | 16, 0x40 | 18, 0x40 | 20, 0x40 | 22, 0x40 | 24, 0x40 | 26, 0x40 | 28, 0x40 | 30,
      0x40 | 32, 0x40 | 34, 0x40 | 36, 0x40 | 38, 0x40 | 40, 0x40 | 42, 0x40 | 44, 0x40 | 46,
      0x40 | 48, 0x40 | 50, 0x40 | 52, 0x40 | 54, 0x40 | 56, 0x40 | 58, 0x40 | 60, 0x40 | 62};
  const __m512i permute_index_v = _mm512_loadu_si512(permute_index);
  // setr_epi8 not exists
  // const __m512i permute_index_v = _mm512_set_epi8(
  //     60 | 0x40, 56 | 0x40, 52 | 0x40, 48 | 0x40, 60, 56, 52, 48,
  //     44 | 0x40, 40 | 0x40, 36 | 0x40, 32 | 0x40, 44, 40, 36, 32,
  //     28 | 0x40, 24 | 0x40, 20 | 0x40, 16 | 0x40, 28, 24, 20, 16,
  //     12 | 0x40,  8 | 0x40,  4 | 0x40,  0 | 0x40, 12, 8, 4, 0,
  //     60 | 0x40, 56 | 0x40, 52 | 0x40, 48 | 0x40, 60, 56, 52, 48,
  //     44 | 0x40, 40 | 0x40, 36 | 0x40, 32 | 0x40, 44, 40, 36, 32,
  //     28 | 0x40, 24 | 0x40, 20 | 0x40, 16 | 0x40, 28, 24, 20, 16,
  //     12 | 0x40,  8 | 0x40,  4 | 0x40,  0 | 0x40, 12, 8, 4, 0);

  for (int i = 0; i < data_size; i += step) {
    uint16_t* sptri   = src + i;
    __m512i src_sub_v = _mm512_min_epu16(
        _mm512_subs_epu16(_mm512_loadu_epi16(sptri), lut_min_v), uint8_max_div_coeff_v);
    __m512i dst_v1 = _mm512_srli_epi16(_mm512_mullo_epi16(src_sub_v, coeff_v), 8);

    // half
    src_sub_v =
        _mm512_min_epu16(_mm512_subs_epu16(_mm512_loadu_epi16(sptri + half_step), lut_min_v),
                         uint8_max_div_coeff_v);
    __m512i dst_v2 = _mm512_srli_epi16(_mm512_mullo_epi16(src_sub_v, coeff_v), 8);

    __m512i dst_v = _mm512_permutex2var_epi8(dst_v1, permute_index_v, dst_v2);
    _mm512_storeu_epi32(dst + i, dst_v);
  }
}

template <> // requires AVX-512 VBMI: Cannon Lake or Tager Lake later or Zen4 or Zen4 // WIP
void LUT::Convert_Impl<LUT::Method::avx512vbmi_calc_intweight_epi32>(uint16_t* src,
                                                                     uint8_t* dst,
                                                                     int32_t data_size) {
  // assert(false);
  constexpr int32_t step      = 512 / 8 / sizeof(uint8_t);
  constexpr int32_t half_step = step >> 1;

  const __m512i coeff_v           = _mm512_set1_epi32(coeff_ * 256);
  const __m512i lut_min_v         = _mm512_set1_epi16(lut_min_);
  const __m512i uint8_max_v       = _mm512_set1_epi32(255 * 256);
  const __m512i zero_v            = _mm512_setzero_si512();
  const uint8_t permute_index[64] = {
      1,  5,  9,  13, 0x40 | 1,  0x40 | 5,  0x40 | 9,  0x40 | 13,
      17, 21, 25, 29, 0x40 | 17, 0x40 | 21, 0x40 | 25, 0x40 | 29,
      33, 37, 41, 45, 0x40 | 33, 0x40 | 37, 0x40 | 41, 0x40 | 45,
      49, 53, 57, 61, 0x40 | 49, 0x40 | 53, 0x40 | 57, 0x40 | 61,
      1,  5,  9,  13, 0x40 | 1,  0x40 | 5,  0x40 | 9,  0x40 | 13,
      17, 21, 25, 29, 0x40 | 17, 0x40 | 21, 0x40 | 25, 0x40 | 29,
      33, 37, 41, 45, 0x40 | 33, 0x40 | 37, 0x40 | 41, 0x40 | 45,
      49, 53, 57, 61, 0x40 | 49, 0x40 | 53, 0x40 | 57, 0x40 | 61};
  const __m512i permute_index_v = _mm512_loadu_si512(permute_index);
  // setr_epi8 not exists
  // const __m512i permute_index_v = _mm512_set_epi8(
  //     60 | 0x40, 56 | 0x40, 52 | 0x40, 48 | 0x40, 60, 56, 52, 48,
  //     44 | 0x40, 40 | 0x40, 36 | 0x40, 32 | 0x40, 44, 40, 36, 32,
  //     28 | 0x40, 24 | 0x40, 20 | 0x40, 16 | 0x40, 28, 24, 20, 16,
  //     12 | 0x40,  8 | 0x40,  4 | 0x40,  0 | 0x40, 12, 8, 4, 0,
  //     60 | 0x40, 56 | 0x40, 52 | 0x40, 48 | 0x40, 60, 56, 52, 48,
  //     44 | 0x40, 40 | 0x40, 36 | 0x40, 32 | 0x40, 44, 40, 36, 32,
  //     28 | 0x40, 24 | 0x40, 20 | 0x40, 16 | 0x40, 28, 24, 20, 16,
  //     12 | 0x40,  8 | 0x40,  4 | 0x40,  0 | 0x40, 12, 8, 4, 0);

  for (int i = 0; i < data_size; i += step) {
    uint16_t* sptri   = src + i;
    __m512i src_sub_v = _mm512_subs_epu16(_mm512_loadu_epi16(sptri), lut_min_v);
    __m512i srcs_hi   = _mm512_unpackhi_epi16(src_sub_v, zero_v);
    __m512i srcs_lo   = _mm512_unpacklo_epi16(src_sub_v, zero_v);
    __m512i val_hi    = _mm512_mullo_epi32(coeff_v, srcs_hi);
    __m512i hi        = _mm512_min_epi32(val_hi, uint8_max_v);
    __m512i val_lo    = _mm512_mullo_epi32(coeff_v, srcs_lo);
    __m512i lo        = _mm512_min_epi32(val_lo, uint8_max_v);

    __m512i dst_v1 = _mm512_permutex2var_epi8(lo, permute_index_v, hi);

    src_sub_v      = _mm512_subs_epu16(_mm512_loadu_epi16(sptri + half_step), lut_min_v);
    srcs_hi        = _mm512_unpackhi_epi16(src_sub_v, zero_v);
    srcs_lo        = _mm512_unpacklo_epi16(src_sub_v, zero_v);
    val_hi         = _mm512_mullo_epi32(coeff_v, srcs_hi);
    hi             = _mm512_min_epi32(val_hi, uint8_max_v);
    val_lo         = _mm512_mullo_epi32(coeff_v, srcs_lo);
    lo             = _mm512_min_epi32(val_lo, uint8_max_v);
    __m512i dst_v2 = _mm512_permutex2var_epi8(lo, permute_index_v, hi);

    _mm512_storeu_epi8(dst + i, _mm512_mask_blend_epi32(0b1111111100000000, dst_v1, dst_v2));
  }
}
