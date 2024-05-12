#include "lut.h"

#include <algorithm>

#include <immintrin.h>

#include <InstructionInfo.h>

LUT::LUT(int32_t range_max) : range_max_(range_max) {
  // lut_       = std::shared_ptr<uint32_t[]>(new (std::align_val_t(64)) uint32_t[range_max]);
  lut_ = std::shared_ptr<uint32_t[]>(new uint32_t[range_max]);
  ImplSelector();
}

template <>
void LUT::Create_Impl<LUT::Method::naive_lut>(int32_t lut_min, int32_t lut_max) {
  uint32_t* lptr = lut_.get();
  float coeff    = 255.0 / (lut_max - lut_min);
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
  coeff_   = 255.0 / (lut_max - lut_min);
  lut_min_ = lut_min;
}

template <>
void LUT::Convert_Impl<LUT::Method::naive_calc>(uint16_t* src, uint8_t* dst,
                                                int32_t data_size) {
  for (int i = 0; i < data_size; i++) {
    dst[i] = std::clamp(static_cast<int32_t>(coeff_ * (src[i] - lut_min_)), 0, 255);
  }
}

template <>
void LUT::Create_Impl<LUT::Method::avx2_lut>(int32_t lut_min, int32_t lut_max) {
  constexpr int32_t step = 256 / 8 / sizeof(uint32_t);
  uint32_t* lptr         = lut_.get();

  const float coeff = 255.0 / (lut_max - lut_min);

  const __m256 index_v = _mm256_setr_ps(0, 1, 2, 3, 4, 5, 6, 7);

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
void LUT::Convert_Impl<LUT::Method::avx2_lut>(uint16_t* src, uint8_t* dst, int32_t data_size) {
  constexpr int32_t step      = 256 / 8 / sizeof(uint8_t);
  constexpr int32_t half_step = step >> 1;
  int32_t* lptr               = reinterpret_cast<int32_t*>(lut_.get());

  const __m256i zero_v = _mm256_setzero_si256();

  // int8_t shuffle_index[32]     = {0, 4, 8, 12, 0, 4, 8, 12, 0, 4, 8, 12, 0, 4, 8, 12,
  //                                 0, 4, 8, 12, 0, 4, 8, 12, 0, 4, 8, 12, 0, 4, 8, 12
  // };
  const __m256i shuffle_idx = _mm256_set1_epi32(0x0C080400);

  for (int i = 0; i < data_size; i += step) {
    uint16_t* sptri     = src + i;
    uint8_t* dptri      = dst + i;
    __m256i src_v       = _mm256_loadu_epi16(sptri);
    __m256i idx_hi      = _mm256_unpackhi_epi16(src_v, zero_v);
    __m256i idx_lo      = _mm256_unpacklo_epi16(src_v, zero_v);
    __m256i hi          = _mm256_i32gather_epi32(lptr, idx_hi, sizeof(uint32_t));
    __m256i lo          = _mm256_i32gather_epi32(lptr, idx_lo, sizeof(uint32_t));
    __m256i compress_hi = _mm256_shuffle_epi8(hi, shuffle_idx);
    __m256i compress_lo = _mm256_shuffle_epi8(lo, shuffle_idx);
    __m256i dst_lo_v    = _mm256_blend_epi32(compress_lo, compress_hi, 0b10101010);
    dst_lo_v            = _mm256_permute4x64_epi64(dst_lo_v, _MM_SHUFFLE(2, 1, 3, 0));

    src_v            = _mm256_loadu_epi16(sptri + half_step);
    idx_hi           = _mm256_unpackhi_epi16(src_v, zero_v);
    idx_lo           = _mm256_unpacklo_epi16(src_v, zero_v);
    hi               = _mm256_i32gather_epi32(lptr, idx_hi, sizeof(uint32_t));
    lo               = _mm256_i32gather_epi32(lptr, idx_lo, sizeof(uint32_t));
    compress_hi      = _mm256_shuffle_epi8(hi, shuffle_idx);
    compress_lo      = _mm256_shuffle_epi8(lo, shuffle_idx);
    __m256i dst_hi_v = _mm256_blend_epi32(compress_lo, compress_hi, 0b10101010);
    dst_hi_v         = _mm256_permute4x64_epi64(dst_hi_v, _MM_SHUFFLE(2, 1, 3, 0));

    dst_lo_v = _mm256_blend_epi32(dst_lo_v, dst_hi_v, 0b11110000);

    _mm256_storeu_epi32(dptri, dst_lo_v);
  }
}

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

template <>
void LUT::Convert_Impl<LUT::Method::avx2_calc>(uint16_t* src, uint8_t* dst, int32_t data_size) {
  constexpr int32_t step      = 256 / 8 / sizeof(uint8_t);
  constexpr int32_t half_step = step >> 1;

  __m256 coeff_v      = _mm256_set1_ps(coeff_);
  __m256i lut_min_v   = _mm256_set1_epi16(lut_min_);
  __m256i uint8_max_v = _mm256_set1_epi32(255);
  __m256i zero_v      = _mm256_setzero_si256();

  const __m256i shuffle_idx = _mm256_set1_epi32(0x0C080400);
  for (int i = 0; i < data_size; i += step) {
    uint16_t* sptri   = src + i;
    uint8_t* dptri    = dst + i;
    __m256i src_sub_v = _mm256_subs_epu16(_mm256_loadu_epi16(sptri), lut_min_v);
    __m256i srcs_hi   = _mm256_unpackhi_epi16(src_sub_v, zero_v);
    __m256i srcs_lo   = _mm256_unpacklo_epi16(src_sub_v, zero_v);

    __m256i val_hi = _mm256_cvtps_epi32(_mm256_mul_ps(coeff_v, _mm256_cvtepi32_ps(srcs_hi)));
    __m256i hi     = _mm256_min_epi32(val_hi, uint8_max_v);
    __m256i val_lo = _mm256_cvtps_epi32(_mm256_mul_ps(coeff_v, _mm256_cvtepi32_ps(srcs_lo)));
    __m256i lo     = _mm256_min_epi32(val_lo, uint8_max_v);

    __m256i compress_hi = _mm256_shuffle_epi8(hi, shuffle_idx);
    __m256i compress_lo = _mm256_shuffle_epi8(lo, shuffle_idx);
    __m256i dst_lo_v    = _mm256_blend_epi32(compress_lo, compress_hi, 0b10101010);
    dst_lo_v            = _mm256_permute4x64_epi64(dst_lo_v, _MM_SHUFFLE(2, 1, 3, 0));

    // half
    src_sub_v = _mm256_subs_epu16(_mm256_loadu_epi16(sptri + half_step), lut_min_v);
    srcs_hi   = _mm256_unpackhi_epi16(src_sub_v, zero_v);
    srcs_lo   = _mm256_unpacklo_epi16(src_sub_v, zero_v);

    val_hi = _mm256_cvtps_epi32(_mm256_mul_ps(coeff_v, _mm256_cvtepi32_ps(srcs_hi)));
    hi     = _mm256_max_epi32(_mm256_min_epi32(val_hi, uint8_max_v), zero_v);
    val_lo = _mm256_cvtps_epi32(_mm256_mul_ps(coeff_v, _mm256_cvtepi32_ps(srcs_lo)));
    lo     = _mm256_max_epi32(_mm256_min_epi32(val_lo, uint8_max_v), zero_v);

    compress_hi      = _mm256_shuffle_epi8(hi, shuffle_idx);
    compress_lo      = _mm256_shuffle_epi8(lo, shuffle_idx);
    __m256i dst_hi_v = _mm256_blend_epi32(compress_lo, compress_hi, 0b10101010);
    dst_hi_v         = _mm256_permute4x64_epi64(dst_hi_v, _MM_SHUFFLE(2, 1, 3, 0));

    dst_lo_v = _mm256_blend_epi32(dst_lo_v, dst_hi_v, 0b11110000);

    _mm256_storeu_epi32(dptri, dst_lo_v);
  }
}

template <> // requires AVX-512f
void LUT::Convert_Impl<LUT::Method::avx512f_calc>(uint16_t* src, uint8_t* dst,
                                                  int32_t data_size) {
  constexpr int32_t step      = 512 / 8 / sizeof(uint8_t);
  constexpr int32_t half_step = step >> 1;
  int32_t* lptr               = reinterpret_cast<int32_t*>(lut_.get());

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

template <> // requires AVX-512 VBMI: Cannon Lake or Tager Lake later or Zen4 or Zen4
void LUT::Convert_Impl<LUT::Method::avx512vbmi_calc>(uint16_t* src, uint8_t* dst,
                                                     int32_t data_size) {
  constexpr int32_t step      = 512 / 8 / sizeof(uint8_t);
  constexpr int32_t half_step = step >> 1;
  int32_t* lptr               = reinterpret_cast<int32_t*>(lut_.get());

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
    uint8_t* dptri    = dst + i;
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

    _mm512_storeu_epi8(dptri, _mm512_mask_blend_epi32(0b1111111100000000, dst_v1, dst_v2));
  }
}

void LUT::ImplSelector() {
  if (InstructionInfo::IsSupported(InstructionInfo::InstructionSet::AVX512_VBMI)) {
    Create_AutoImpl  = &LUT::Create_Impl<Method::naive_calc>;
    Convert_AutoImpl = &LUT::Convert_Impl<Method::avx512vbmi_calc>;
  } else if (InstructionInfo::IsSupported(InstructionInfo::InstructionSet::AVX512F)) {
    Create_AutoImpl  = &LUT::Create_Impl<Method::naive_calc>;
    Convert_AutoImpl = &LUT::Convert_Impl<Method::avx512f_calc>;
  } else if (InstructionInfo::IsSupported(InstructionInfo::InstructionSet::AVX2)) {
    Create_AutoImpl  = &LUT::Create_Impl<Method::avx2_lut>;
    Convert_AutoImpl = &LUT::Convert_Impl<Method::avx2_lut>;
  } else {
    Create_AutoImpl  = &LUT::Create_Impl<Method::naive_lut>;
    Convert_AutoImpl = &LUT::Convert_Impl<Method::naive_lut>;
  }
}
