#include "vhadd.h"

#include <algorithm>
#include <cassert>
#include <expected>
#include <ranges>

#include <immintrin.h>

constexpr VHAdd::Method VMA512 = VHAdd::Method::AVX512;

constexpr int32_t step      = 512 / 8 / sizeof(uint16_t);
constexpr int32_t half_step = step >> 1;

template<>
std::span<uint16_t> VHAdd::CalcV_Impl<VMA512>(uint16_t* src, int32_t size, int32_t offset_x, int32_t offset_y,
                                              int32_t horizontal, int32_t vertical) {
  assert(width_ * height_ == size);
  assert((offset_x + horizontal) <= width_);
  assert((offset_y + vertical) <= height_);

  std::span<int32_t> acc = std::span<int32_t>(vaptr_ + offset_x, horizontal + step - 1);
  result_slice_[0]       = std::span<uint16_t>(vdptr_ + offset_x, horizontal);
  std::ranges::fill(acc, 0);

  __m512i zero_v = _mm512_setzero_si512();
  for (auto j : std::views::iota(offset_y, offset_y + vertical)) {
    uint16_t* sptrj = src + width_ * j;
    for (int32_t i = offset_x; i < offset_x + horizontal; i += step) {
      int32_t* vaptri = vaptr_ + i;
      __m512i spji    = _mm512_loadu_epi32(sptrj + i);
      __m512i vailo   = _mm512_loadu_epi32(vaptri);
      __m512i vaihi   = _mm512_loadu_epi32(vaptri + half_step);
      __m512i lo      = _mm512_unpacklo_epi16(spji, zero_v);
      __m512i hi      = _mm512_unpackhi_epi16(spji, zero_v);
      lo              = _mm512_add_epi32(vailo, lo);
      hi              = _mm512_add_epi32(vaihi, hi);
      _mm512_storeu_epi32(vaptri, lo);
      _mm512_storeu_epi32(vaptri + half_step, hi);
    }
  }

  float r   = CalcRcp(vertical);
  __m512 rv = _mm512_set1_ps(r);
  for (int32_t i = offset_x; i < offset_x + horizontal; i += step) {
    int32_t* vaptri = vaptr_ + i;
    __m512 vailo    = _mm512_cvtepi32_ps(_mm512_loadu_epi32(vaptri));
    __m512 vaihi    = _mm512_cvtepi32_ps(_mm512_loadu_epi32(vaptri + half_step));
    __m512i lo      = _mm512_cvttps_epi32(_mm512_mul_ps(vailo, rv));
    __m512i hi      = _mm512_cvttps_epi32(_mm512_mul_ps(vaihi, rv));
    _mm512_storeu_epi32(vdptr_ + i, _mm512_packus_epi32(lo, hi));
  }

  return result_slice_[0];
}

template<>
std::span<uint16_t> VHAdd::CalcH_Impl<VMA512>(uint16_t* src, int32_t size, int32_t offset_x, int32_t offset_y,
                                              int32_t horizontal, int32_t vertical) {
  assert(width_ * height_ == size);
  assert((offset_x + horizontal) <= width_);
  assert((offset_y + vertical) <= height_);

  result_slice_[1] = std::span<uint16_t>(hdptr_ + offset_y, vertical);

  float r        = CalcRcp(horizontal);
  __m512i zero_v = _mm512_setzero_si512();
  for (auto j : std::views::iota(offset_y, offset_y + vertical)) {
    uint16_t* sptrj = src + width_ * j;
    __m512i acc     = _mm512_setzero_si512();
    int32_t i       = offset_x;
    for (; i < offset_x + horizontal - step + 1; i += step) {
      __m512i spji = _mm512_loadu_epi32(sptrj + i);
      __m512i lo   = _mm512_unpacklo_epi16(spji, zero_v);
      __m512i hi   = _mm512_unpackhi_epi16(spji, zero_v);
      acc          = _mm512_add_epi32(acc, _mm512_add_epi32(lo, hi));
    }
    __m512i cross = _mm512_shuffle_epi32(acc, _MM_PERM_BADC);
    acc           = _mm512_add_epi32(acc, cross);
    cross         = _mm512_shuffle_epi32(acc, _MM_PERM_CDAB);
    acc           = _mm512_add_epi32(acc, cross);
    int32_t v     = reinterpret_cast<int32_t*>(&acc)[0] + reinterpret_cast<int32_t*>(&acc)[4] +
                reinterpret_cast<int32_t*>(&acc)[8] + reinterpret_cast<int32_t*>(&acc)[12];
    for (; i < offset_x + horizontal; i++) {
      v += sptrj[i];
    }
    hdptr_[j] = static_cast<uint16_t>(static_cast<float>(v) * r);
  }

  return result_slice_[1];
}

template<>
std::array<std::span<uint16_t>, 2> VHAdd::CalcVH_Impl<VMA512>(uint16_t* src, int32_t size, int32_t offset_x,
                                                              int32_t offset_y, int32_t horizontal, int32_t vertical) {
  assert(width_ * height_ == size);
  assert((offset_x + horizontal) <= width_);
  assert((offset_y + vertical) <= height_);

  std::span<int32_t> acc = std::span<int32_t>(vaptr_ + offset_x, horizontal + step - 1);
  result_slice_[0]       = std::span<uint16_t>(vdptr_ + offset_x, horizontal);
  result_slice_[1]       = std::span<uint16_t>(hdptr_ + offset_y, vertical);
  std::ranges::fill(acc, 0);

  float hr       = CalcRcp(horizontal);
  __m512i zero_v = _mm512_setzero_si512();
  for (auto j : std::views::iota(offset_y, offset_y + vertical)) {
    uint16_t* sptrj = src + width_ * j;

    __m512i acc = _mm512_setzero_si512(); // h
    int32_t i   = offset_x;
    for (; i < offset_x + horizontal - step + 1; i += step) {
      int32_t* vaptri = vaptr_ + i;                                      // v
      __m512i spji    = _mm512_loadu_epi32(sptrj + i);                   // vh
      __m512i vailo   = _mm512_loadu_epi32(vaptri);                      // v
      __m512i vaihi   = _mm512_loadu_epi32(vaptri + half_step);          // v
      __m512i lo      = _mm512_unpacklo_epi16(spji, zero_v);             // vh
      __m512i hi      = _mm512_unpackhi_epi16(spji, zero_v);             // vh
      acc             = _mm512_add_epi32(acc, _mm512_add_epi32(lo, hi)); // h
      lo              = _mm512_add_epi32(vailo, lo);                     // v
      hi              = _mm512_add_epi32(vaihi, hi);                     // v
      _mm512_storeu_epi32(vaptri, lo);                                   // v
      _mm512_storeu_epi32(vaptri + half_step, hi);                       // v
    }

    // for vertical padding load
    int32_t* vaptri = vaptr_ + i;                             // v
    __m512i spji    = _mm512_loadu_epi32(sptrj + i);          // v
    __m512i vailo   = _mm512_loadu_epi32(vaptri);             // v
    __m512i vaihi   = _mm512_loadu_epi32(vaptri + half_step); // v

    __m512i cross = _mm512_shuffle_epi32(acc, _MM_PERM_BADC); // h
    acc           = _mm512_add_epi32(acc, cross);             // h
    cross         = _mm512_shuffle_epi32(acc, _MM_PERM_CDAB); // h
    acc           = _mm512_add_epi32(acc, cross);             // h
    int32_t v     = reinterpret_cast<int32_t*>(&acc)[0] + reinterpret_cast<int32_t*>(&acc)[4] +
                reinterpret_cast<int32_t*>(&acc)[8] + reinterpret_cast<int32_t*>(&acc)[12]; // h

    // for vertical padding calc
    __m512i lo = _mm512_unpacklo_epi16(spji, zero_v); // v
    __m512i hi = _mm512_unpackhi_epi16(spji, zero_v); // v
    lo         = _mm512_add_epi32(vailo, lo);         // v
    hi         = _mm512_add_epi32(vaihi, hi);         // v
    _mm512_storeu_epi32(vaptri, lo);                  // v
    _mm512_storeu_epi32(vaptri + half_step, hi);      // v

    for (; i < offset_x + horizontal; i++) { // for horizontal padding
      v += sptrj[i];                         // h
    }
    hdptr_[j] = static_cast<uint16_t>(static_cast<float>(v) * hr); // h
  }

  float vr    = CalcRcp(vertical);
  __m512 vr_v = _mm512_set1_ps(vr);                                               // v
  for (int32_t i = offset_x; i < offset_x + horizontal; i += step) {              // v
    int32_t* vaptri = vaptr_ + i;                                                 // v
    __m512 vailo    = _mm512_cvtepi32_ps(_mm512_loadu_epi32(vaptri));             // v
    __m512 vaihi    = _mm512_cvtepi32_ps(_mm512_loadu_epi32(vaptri + half_step)); // v
    __m512i lo      = _mm512_cvttps_epi32(_mm512_mul_ps(vailo, vr_v));            // v
    __m512i hi      = _mm512_cvttps_epi32(_mm512_mul_ps(vaihi, vr_v));            // v
    _mm512_storeu_epi32(vdptr_ + i, _mm512_packus_epi32(lo, hi));                 // v
  }

  return result_slice_;
}
