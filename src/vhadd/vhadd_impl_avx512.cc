#include "vhadd.h"

#include <expected>
#include <algorithm>
#include <cassert>
#include <ranges>

#include <immintrin.h>

constexpr VHAdd::Method VMA512  = VHAdd::Method::AVX512;

constexpr int32_t step      = 512 / 8 / sizeof(uint16_t);
constexpr int32_t half_step = step >> 1;

template <>
std::span<uint16_t> VHAdd::CalcV_Impl<VMA512>(uint16_t* src, int32_t size, int32_t offset_x,
                                            int32_t offset_y, int32_t horizontal,
                                            int32_t vertical) {
  assert(width_ * height_ == size);
  assert((offset_x + horizontal) <= width_);
  assert((offset_y + vertical) <= height_);

  std::span<int32_t> acc = std::span<int32_t>(vaptr_ + offset_x, horizontal + step - 1);
  result_slice_[0]       = std::span<uint16_t>(vdptr_ + offset_x, horizontal);
  std::ranges::fill(acc, 0);

  __m256i zero_v = _mm256_setzero_si256();
  for (auto j : std::views::iota(offset_y, offset_y + vertical)) {
    uint16_t* sptrj = src + width_ * j;
    for (int32_t i = offset_x; i < offset_x + horizontal; i += step) {
      int32_t* vaptri = vaptr_ + i;
      __m256i spji    = _mm256_loadu_epi32(sptrj + i);
      __m256i vailo   = _mm256_loadu_epi32(vaptri);
      __m256i vaihi   = _mm256_loadu_epi32(vaptri + half_step);
      __m256i lo      = _mm256_unpacklo_epi16(spji, zero_v);
      __m256i hi      = _mm256_unpackhi_epi16(spji, zero_v);
      lo              = _mm256_add_epi32(vailo, lo);
      hi              = _mm256_add_epi32(vaihi, hi);
      _mm256_storeu_epi32(vaptri, lo);
      _mm256_storeu_epi32(vaptri + half_step, hi);
    }
  }

  float r   = CalcRcp(vertical);
  __m256 rv = _mm256_set1_ps(r);
  for (int32_t i = offset_x; i < offset_x + horizontal; i += step) {
    int32_t* vaptri = vaptr_ + i;
    __m256 vailo    = _mm256_cvtepi32_ps(_mm256_loadu_epi32(vaptri));
    __m256 vaihi    = _mm256_cvtepi32_ps(_mm256_loadu_epi32(vaptri + half_step));
    __m256i lo      = _mm256_cvttps_epi32(_mm256_mul_ps(vailo, rv));
    __m256i hi      = _mm256_cvttps_epi32(_mm256_mul_ps(vaihi, rv));
    _mm256_storeu_epi32(vdptr_ + i, _mm256_packus_epi32(lo, hi));
  }

  return result_slice_[0];
}

template <>
std::span<uint16_t> VHAdd::CalcH_Impl<VMA512>(uint16_t* src, int32_t size, int32_t offset_x,
                                            int32_t offset_y, int32_t horizontal,
                                            int32_t vertical) {
  assert(width_ * height_ == size);
  assert((offset_x + horizontal) <= width_);
  assert((offset_y + vertical) <= height_);

  result_slice_[1] = std::span<uint16_t>(hdptr_ + offset_y, vertical);

  float r        = CalcRcp(horizontal);
  __m256i zero_v = _mm256_setzero_si256();
  for (auto j : std::views::iota(offset_y, offset_y + vertical)) {
    uint16_t* sptrj = src + width_ * j;
    __m256i acc     = _mm256_setzero_si256();
    int32_t i       = offset_x;
    for (; i < offset_x + horizontal - step + 1; i += step) {
      __m256i spji = _mm256_loadu_epi32(sptrj + i);
      __m256i lo   = _mm256_unpacklo_epi16(spji, zero_v);
      __m256i hi   = _mm256_unpackhi_epi16(spji, zero_v);
      acc          = _mm256_add_epi32(acc, _mm256_add_epi32(lo, hi));
    }
    __m256i cross = _mm256_shuffle_epi32(acc, _MM_SHUFFLE(1, 0, 3, 2));
    acc           = _mm256_add_epi32(acc, cross);
    cross         = _mm256_shuffle_epi32(acc, _MM_SHUFFLE(2, 3, 0, 1));
    acc           = _mm256_add_epi32(acc, cross);
    int32_t v     = reinterpret_cast<int32_t*>(&acc)[0] + reinterpret_cast<int32_t*>(&acc)[4];
    for (; i < offset_x + horizontal; i++) {
      v += sptrj[i];
    }
    hdptr_[j] = static_cast<uint16_t>(static_cast<float>(v) * r);
  }

  return result_slice_[1];
}

template <>
std::array<std::span<uint16_t>, 2> VHAdd::CalcVH_Impl<VMA512>(uint16_t* src, int32_t size,
                                                            int32_t offset_x, int32_t offset_y,
                                                            int32_t horizontal,
                                                            int32_t vertical) {
  assert(width_ * height_ == size);
  assert((offset_x + horizontal) <= width_);
  assert((offset_y + vertical) <= height_);

  std::span<int32_t> acc = std::span<int32_t>(vaptr_ + offset_x, horizontal + step - 1);
  result_slice_[0]       = std::span<uint16_t>(vdptr_ + offset_x, horizontal);
  result_slice_[1]       = std::span<uint16_t>(hdptr_ + offset_y, vertical);
  std::ranges::fill(acc, 0);

  float hr       = CalcRcp(horizontal);
  __m256i zero_v = _mm256_setzero_si256();
  for (auto j : std::views::iota(offset_y, offset_y + vertical)) {
    uint16_t* sptrj = src + width_ * j;

    __m256i acc = _mm256_setzero_si256(); // h
    int32_t i   = offset_x;
    for (; i < offset_x + horizontal - step + 1; i += step) {
      int32_t* vaptri = vaptr_ + i;                                      // v
      __m256i spji    = _mm256_loadu_epi32(sptrj + i);                   // vh
      __m256i vailo   = _mm256_loadu_epi32(vaptri);                      // v
      __m256i vaihi   = _mm256_loadu_epi32(vaptri + half_step);          // v
      __m256i lo      = _mm256_unpacklo_epi16(spji, zero_v);             // vh
      __m256i hi      = _mm256_unpackhi_epi16(spji, zero_v);             // vh
      acc             = _mm256_add_epi32(acc, _mm256_add_epi32(lo, hi)); // h
      lo              = _mm256_add_epi32(vailo, lo);                     // v
      hi              = _mm256_add_epi32(vaihi, hi);                     // v
      _mm256_storeu_epi32(vaptri, lo);                                   // v
      _mm256_storeu_epi32(vaptri + half_step, hi);                       // v
    }

    // for vertical padding load
    int32_t* vaptri = vaptr_ + i;                             // v
    __m256i spji    = _mm256_loadu_epi32(sptrj + i);          // v
    __m256i vailo   = _mm256_loadu_epi32(vaptri);             // v
    __m256i vaihi   = _mm256_loadu_epi32(vaptri + half_step); // v

    __m256i cross = _mm256_shuffle_epi32(acc, _MM_SHUFFLE(1, 0, 3, 2));                    // h
    acc           = _mm256_add_epi32(acc, cross);                                          // h
    cross         = _mm256_shuffle_epi32(acc, _MM_SHUFFLE(2, 3, 0, 1));                    // h
    acc           = _mm256_add_epi32(acc, cross);                                          // h
    int32_t v = reinterpret_cast<int32_t*>(&acc)[0] + reinterpret_cast<int32_t*>(&acc)[4]; // h

    // for vertical padding calc
    __m256i lo = _mm256_unpacklo_epi16(spji, zero_v); // v
    __m256i hi = _mm256_unpackhi_epi16(spji, zero_v); // v
    lo         = _mm256_add_epi32(vailo, lo);         // v
    hi         = _mm256_add_epi32(vaihi, hi);         // v
    _mm256_storeu_epi32(vaptri, lo);                  // v
    _mm256_storeu_epi32(vaptri + half_step, hi);      // v

    for (; i < offset_x + horizontal; i++) { // for horizontal padding
      v += sptrj[i];                         // h
    }
    hdptr_[j] = static_cast<uint16_t>(static_cast<float>(v) * hr); // h
  }

  float vr    = CalcRcp(vertical);
  __m256 vr_v = _mm256_set1_ps(vr);                                               // v
  for (int32_t i = offset_x; i < offset_x + horizontal; i += step) {              // v
    int32_t* vaptri = vaptr_ + i;                                                 // v
    __m256 vailo    = _mm256_cvtepi32_ps(_mm256_loadu_epi32(vaptri));             // v
    __m256 vaihi    = _mm256_cvtepi32_ps(_mm256_loadu_epi32(vaptri + half_step)); // v
    __m256i lo      = _mm256_cvttps_epi32(_mm256_mul_ps(vailo, vr_v));            // v
    __m256i hi      = _mm256_cvttps_epi32(_mm256_mul_ps(vaihi, vr_v));            // v
    _mm256_storeu_epi32(vdptr_ + i, _mm256_packus_epi32(lo, hi));                 // v
  }

  return result_slice_;
}
