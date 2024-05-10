#include "lut.h"

#include <algorithm>

#include <immintrin.h>

LUT::LUT(int32_t range_max) {
  // lut_       = std::shared_ptr<uint32_t[]>(new (std::align_val_t(64)) uint32_t[range_max]);
  lut_       = std::shared_ptr<uint32_t[]>(new uint32_t[range_max]);
  range_max_ = range_max;
}

template <>
void LUT::Create<LUT::Method::naive_lut>(int32_t lut_min, int32_t lut_max) {
  uint32_t* lptr = lut_.get();
  float coeff    = 255.0 / (lut_max - lut_min);
  for (int i = 0; i < range_max_; i++) {
    lptr[i] = std::clamp(static_cast<int32_t>(coeff * (i - lut_min)), 0, 255);
  }
}

template <>
void LUT::Convert<LUT::Method::naive_lut>(uint16_t* src, uint8_t* dst, int32_t data_size) {
  uint32_t* lptr = lut_.get();
  for (int i = 0; i < data_size; i++) {
    dst[i] = lptr[src[i]];
  }
}

template <>
void LUT::Create<LUT::Method::avx2_lut>(int32_t lut_min, int32_t lut_max) {
  constexpr int32_t step = 256 / 8 / sizeof(uint32_t);
  uint32_t* lptr         = lut_.get();

  const float coeff = 255.0 / (lut_max - lut_min);

  float index[8]       = {0, 1, 2, 3, 4, 5, 6, 7};
  const __m256 index_v = _mm256_loadu_ps(index);

  const __m256i uint8_max_v = _mm256_set1_epi32(255);
  const __m256 lut_min_v    = _mm256_set1_ps(lut_min);
  const __m256i zero_v      = _mm256_setzero_si256();
  const __m256 coeff_v      = _mm256_set1_ps(coeff);

  for (int32_t i = 0; i < range_max_; i += step) {
    const __m256 i_v = _mm256_add_ps(_mm256_set1_ps(i), index_v);
    const __m256i val =
        _mm256_cvtps_epi32(_mm256_mul_ps(coeff_v, _mm256_sub_ps(i_v, lut_min_v)));
    _mm256_storeu_epi32(lptr + i, _mm256_max_epi32(_mm256_min_epi32(val, uint8_max_v), zero_v));
  }
}

template <>
void LUT::Convert<LUT::Method::avx2_lut>(uint16_t* src, uint8_t* dst, int32_t data_size) {
  constexpr int32_t step      = 256 / 8 / sizeof(uint8_t);
  constexpr int32_t half_step = step >> 1;
  int32_t* lptr               = reinterpret_cast<int32_t*>(lut_.get());

  const __m256i zero_v         = _mm256_setzero_si256();

  // int8_t shuffle_index[32]     = {0, 4, 8, 12, 0, 4, 8, 12, 0, 4, 8, 12, 0, 4, 8, 12,
  //                                 0, 4, 8, 12, 0, 4, 8, 12, 0, 4, 8, 12, 0, 4, 8, 12
  // };
  const __m256i shuffle_idx = _mm256_set1_epi32(0x0C080400);

  for (int i = 0; i < data_size; i += step) {
    uint16_t* sptri = src + i;
    uint8_t* dptri  = dst + i;
    __m256i src_v   = _mm256_loadu_epi16(sptri);
    __m256i idx_hi  = _mm256_unpackhi_epi16(src_v, zero_v);
    __m256i idx_lo  = _mm256_unpacklo_epi16(src_v, zero_v);
    __m256i hi      = _mm256_i32gather_epi32(lptr, idx_hi, sizeof(uint32_t));
    __m256i lo      = _mm256_i32gather_epi32(lptr, idx_lo, sizeof(uint32_t));
    __m256i comphi  = _mm256_shuffle_epi8(hi, shuffle_idx);
    __m256i complo  = _mm256_shuffle_epi8(lo, shuffle_idx);
    __m256i dst_lo_v =  _mm256_blend_epi32(complo, comphi, 0b10101010);
    dst_lo_v = _mm256_permute4x64_epi64(dst_lo_v, _MM_SHUFFLE(2, 1, 3, 0));

    src_v  = _mm256_loadu_epi16(sptri + half_step);
    idx_hi = _mm256_unpackhi_epi16(src_v, zero_v);
    idx_lo = _mm256_unpacklo_epi16(src_v, zero_v);
    hi     = _mm256_i32gather_epi32(lptr, idx_hi, sizeof(uint32_t));
    lo     = _mm256_i32gather_epi32(lptr, idx_lo, sizeof(uint32_t));
    comphi  = _mm256_shuffle_epi8(hi, shuffle_idx);
    complo  = _mm256_shuffle_epi8(lo, shuffle_idx);
    __m256i dst_hi_v = _mm256_blend_epi32(complo, comphi, 0b10101010);
    dst_hi_v = _mm256_permute4x64_epi64(dst_hi_v, _MM_SHUFFLE(2, 1, 3, 0));

    dst_lo_v = _mm256_blend_epi32(dst_lo_v, dst_hi_v, 0b11110000);

    _mm256_storeu_epi32(dptri, dst_lo_v);
  }
}

template <>
void LUT::Convert<LUT::Method::avx512_lut>(uint16_t* src, uint8_t* dst, int32_t data_size) {
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
    // permutex2var_epi8

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
