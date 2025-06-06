#include "lut.h"

#include <cmath>
#include <immintrin.h>

// SIMDを用いたLUT作成
template<>
void LUT::Create_Impl<LUT::Method::avx2_lut>(int32_t lut_min, int32_t lut_max) {
  constexpr int32_t step = 256 / 8 / sizeof(uint32_t);
  uint32_t* lptr         = lut_.get();

  const float coeff = 255.0 / (lut_max - lut_min + 1);

  const __m256 index_v = _mm256_setr_ps(0, 1, 2, 3, 4, 5, 6, 7);

  const __m256i uint8_max_v = _mm256_set1_epi32(255);
  const __m256 lut_min_v    = _mm256_set1_ps(lut_min);
  const __m256i zero_v      = _mm256_setzero_si256();
  const __m256 coeff_v      = _mm256_set1_ps(coeff);

  for (int32_t i = 0; i < range_max_; i += step) {
    const __m256 i_v  = _mm256_add_ps(_mm256_set1_ps(i), index_v);
    const __m256i val = _mm256_cvtps_epi32(_mm256_mul_ps(coeff_v, _mm256_sub_ps(i_v, lut_min_v)));
    _mm256_storeu_epi32(lptr + i, _mm256_max_epi32(_mm256_min_epi32(val, uint8_max_v), zero_v));
  }
}

// uint16_t入力をunpackでuint32_tベクトルx2にわけ，gatherでLUT取得しuint8_tへパック
template<>
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

// uint16_t入力をunpackでuint32_tベクトルx2にわけ，係数乗算はfloatで計算
// uint32_tへ変換しuint8_tにパックする 一番 精度が高い
template<>
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

// 係数を整数として扱う．uint16_tのまま計算を行い，uint8_tへパックする
// [要テスト] minmax取る順番を早くしているので発生しない予定だが，桁溢れがこわい
// 式変形
//      dst[i] = clamp(coeff_ * (src[i] - lut_min), 0, 255)
//   -> dst[i] = min(max(coeff * (src[i] - lut_min), 0), 255)
//   -> dst[i] = coeff_ * (min(max(src[i] - lut_min), 0 / coeff), 255 / coeff)
//   -> dst[i] = (coeff_ * 256) * (min(max(src[i] - lut_min), 0), 255 / coeff) / 256
template<>
void LUT::Convert_Impl<LUT::Method::avx2_calc_intweight_epu16>(uint16_t* src, uint8_t* dst, int32_t data_size) {
  constexpr int32_t step      = 256 / 8 / sizeof(uint8_t);
  constexpr int32_t half_step = step >> 1;

  __m256i coeff_v               = _mm256_set1_epi16(std::ceil(coeff_ * (0x100)));
  __m256i lut_min_v             = _mm256_set1_epi16(lut_min_);
  __m256i uint8_max_div_coeff_v = _mm256_set1_epi16((255.0 / coeff_));

  const __m256i shuffle_idx = _mm256_set1_epi64x(0x0E0C0A0806040200);
  for (int i = 0; i < data_size; i += step) {
    uint16_t* sptri = src + i;
    uint8_t* dptri  = dst + i;
    __m256i src_sub_v =
        _mm256_min_epu16(_mm256_subs_epu16(_mm256_loadu_epi16(sptri), lut_min_v), uint8_max_div_coeff_v);
    __m256i dst_v1 = _mm256_srli_epi16(_mm256_mullo_epi16(src_sub_v, coeff_v), 8);

    dst_v1 = _mm256_shuffle_epi8(dst_v1, shuffle_idx);
    dst_v1 = _mm256_permute4x64_epi64(dst_v1, _MM_SHUFFLE(2, 1, 3, 0));

    // half
    src_sub_v =
        _mm256_min_epu16(_mm256_subs_epu16(_mm256_loadu_epi16(sptri + half_step), lut_min_v), uint8_max_div_coeff_v);
    __m256i dst_v2 = _mm256_srli_epi16(_mm256_mullo_epi16(src_sub_v, coeff_v), 8);

    dst_v2 = _mm256_shuffle_epi8(dst_v2, shuffle_idx);
    dst_v2 = _mm256_permute4x64_epi64(dst_v2, _MM_SHUFFLE(2, 1, 3, 0));

    __m256i dst_v = _mm256_blend_epi32(dst_v1, dst_v2, 0b11110000);
    _mm256_storeu_epi32(dptri, dst_v);
  }
}

// 係数を整数として扱う．uint32_tへキャストしuint16_tへ戻し，uint8_tへパックする
// 桁溢れの心配が少ない intweight_epu16 が完成するまで最速だったもの
template<>
void LUT::Convert_Impl<LUT::Method::avx2_calc_intweight_epi32>(uint16_t* src, uint8_t* dst, int32_t data_size) {
  constexpr int32_t step      = 256 / 8 / sizeof(uint8_t);
  constexpr int32_t half_step = step >> 1;

  __m256i coeff_v     = _mm256_set1_epi32(coeff_ * 256);
  __m256i lut_min_v   = _mm256_set1_epi16(lut_min_);
  __m256i uint8_max_v = _mm256_set1_epi32(255 * 256);
  __m256i zero_v      = _mm256_setzero_si256();

  const __m256i shuffle_idx = _mm256_set1_epi32(0x0D090501);
  for (int i = 0; i < data_size; i += step) {
    uint16_t* sptri   = src + i;
    uint8_t* dptri    = dst + i;
    __m256i src_sub_v = _mm256_subs_epu16(_mm256_loadu_epi16(sptri), lut_min_v);
    __m256i srcs_hi   = _mm256_unpackhi_epi16(src_sub_v, zero_v);
    __m256i srcs_lo   = _mm256_unpacklo_epi16(src_sub_v, zero_v);

    __m256i val_hi = _mm256_mullo_epi32(coeff_v, srcs_hi);
    __m256i hi     = _mm256_min_epi32(val_hi, uint8_max_v);
    __m256i val_lo = _mm256_mullo_epi32(coeff_v, srcs_lo);
    __m256i lo     = _mm256_min_epi32(val_lo, uint8_max_v);

    __m256i compress_hi = _mm256_shuffle_epi8(hi, shuffle_idx);
    __m256i compress_lo = _mm256_shuffle_epi8(lo, shuffle_idx);
    __m256i dst_lo_v    = _mm256_blend_epi32(compress_lo, compress_hi, 0b10101010);
    dst_lo_v            = _mm256_permute4x64_epi64(dst_lo_v, _MM_SHUFFLE(2, 1, 3, 0));

    // half
    src_sub_v = _mm256_subs_epu16(_mm256_loadu_epi16(sptri + half_step), lut_min_v);
    srcs_hi   = _mm256_unpackhi_epi16(src_sub_v, zero_v);
    srcs_lo   = _mm256_unpacklo_epi16(src_sub_v, zero_v);

    val_hi = _mm256_mullo_epi32(coeff_v, srcs_hi);
    hi     = _mm256_max_epi32(_mm256_min_epi32(val_hi, uint8_max_v), zero_v);
    val_lo = _mm256_mullo_epi32(coeff_v, srcs_lo);
    lo     = _mm256_max_epi32(_mm256_min_epi32(val_lo, uint8_max_v), zero_v);

    compress_hi      = _mm256_shuffle_epi8(hi, shuffle_idx);
    compress_lo      = _mm256_shuffle_epi8(lo, shuffle_idx);
    __m256i dst_hi_v = _mm256_blend_epi32(compress_lo, compress_hi, 0b10101010);
    dst_hi_v         = _mm256_permute4x64_epi64(dst_hi_v, _MM_SHUFFLE(2, 1, 3, 0));

    dst_lo_v = _mm256_blend_epi32(dst_lo_v, dst_hi_v, 0b11110000);

    _mm256_storeu_epi32(dptri, dst_lo_v);
  }
}
