#include "histo.h"

#include <ranges>

#include <omp.h>
#include <immintrin.h>

template <>
void MyHisto::Create_Impl<MyHisto::Method::Naive>(uint16_t* source, int32_t data_size) {
  std::ranges::fill(histo_, 0);
  for (auto i : std::views::iota(0, data_size)) {
    histo_[source[i]]++;
  }
}

template <>
void MyHisto::Create_Impl<MyHisto::Method::AVX512VPOPCNTDQ>(uint16_t* source,
                                                            int32_t data_size) {
  std::ranges::fill(histo_, 0);

  constexpr int32_t step         = 512 / 8 / sizeof(uint16_t);
  constexpr int32_t half_step    = step >> 1;
  constexpr int32_t gather_scale = sizeof(int32_t);

  const __m512i zero_v = _mm512_setzero_si512();
  const __m512i one_v  = _mm512_set1_epi32(1);

  int32_t* hptr = histo_.data();

  const int32_t loop_end = data_size - step + 1;
  int32_t i              = 0;
  for (; i < loop_end; i += step) {
    const __m512i src_v = _mm512_loadu_si512(source + i);

    __m512i src_half  = _mm512_unpacklo_epi16(src_v, zero_v);
    __m512i conflict  = _mm512_conflict_epi32(src_half);
    __m512i histo_val = _mm512_i32gather_epi32(src_half, hptr, gather_scale);
    conflict          = _mm512_popcnt_epi32(conflict);
    histo_val         = _mm512_add_epi32(_mm512_add_epi32(histo_val, conflict), one_v);
    _mm512_i32scatter_epi32(hptr, src_half, histo_val, gather_scale);

    src_half  = _mm512_unpackhi_epi16(src_v, zero_v);
    conflict  = _mm512_conflict_epi32(src_half);
    histo_val = _mm512_i32gather_epi32(src_half, hptr, gather_scale);
    conflict  = _mm512_popcnt_epi32(conflict);
    histo_val = _mm512_add_epi32(_mm512_add_epi32(histo_val, conflict), one_v);
    _mm512_i32scatter_epi32(hptr, src_half, histo_val, gather_scale);
  }
}

template <>
void MyHisto::Create_Impl<MyHisto::Method::AVX512VPOPCNTDQ_Order>(uint16_t* source,
                                                                  int32_t data_size) {
  std::ranges::fill(histo_, 0);

  constexpr int32_t step         = 512 / 8 / sizeof(uint16_t);
  constexpr int32_t half_step    = step >> 1;
  constexpr int32_t gather_scale = sizeof(int32_t);

  const __m512i zero_v = _mm512_setzero_si512();
  const __m512i one_v  = _mm512_set1_epi32(1);

  int32_t* hptr = histo_.data();

  const int32_t loop_end = data_size - step + 1;
  int32_t i              = 0;

  for (; i < loop_end; i += step) {
    __m512i src_v       = _mm512_loadu_si512(source + i);
    __m512i src_lo      = _mm512_unpacklo_epi16(src_v, zero_v);
    __m512i src_hi      = _mm512_unpackhi_epi16(src_v, zero_v);
    __m512i conflict_lo = _mm512_conflict_epi32(src_lo);
    __m512i conflict_hi = _mm512_conflict_epi32(src_hi);
    conflict_lo         = _mm512_popcnt_epi32(conflict_lo);
    conflict_hi         = _mm512_popcnt_epi32(conflict_hi);
    conflict_lo         = _mm512_add_epi32(conflict_lo, one_v);
    conflict_hi         = _mm512_add_epi32(conflict_hi, one_v);

    __m512i histo_val_lo = _mm512_i32gather_epi32(src_lo, hptr, gather_scale);
    histo_val_lo         = _mm512_add_epi32(histo_val_lo, conflict_lo);
    _mm512_i32scatter_epi32(hptr, src_lo, histo_val_lo, gather_scale);

    __m512i histo_val_hi = _mm512_i32gather_epi32(src_hi, hptr, gather_scale);
    histo_val_hi         = _mm512_add_epi32(histo_val_hi, conflict_hi);
    _mm512_i32scatter_epi32(hptr, src_hi, histo_val_hi, gather_scale);
  }
}

// TODO: Multi Naive + Naive Conv
template <>
void MyHisto::Create_Impl<MyHisto::Method::Naive_MultiSubloop>(uint16_t* source,
                                                               int32_t data_size) {
  const int32_t range_max     = histo_.size();
  const int32_t sub_data_end  = data_size / parallel_size_;
  const int32_t sub_range_end = range_max / parallel_size_;
  std::ranges::fill(histo_all_, 0);

  int32_t* hptr = histo_.data();
#pragma omp parallel for
  for (int32_t i = 0; i < parallel_size_; i++) {
    uint16_t* sptr = source + i * sub_data_end;
    int32_t* hptri = hptr + i * range_max;
#pragma omp unroll
    for (int32_t j = 0; j < sub_data_end; j++) {
      hptri[sptr[j]]++;
    }
  }

#pragma omp unroll
  for (int32_t p = 1; p < parallel_size_; p++) {
#pragma omp parallel for
    for (int32_t i = 0; i < parallel_size_; i++) {
      int32_t* histo_i = hptr + i * sub_range_end;
      int32_t* hptrp   = hptr + p * range_max + i * sub_range_end;
#pragma omp unroll
      for (int32_t j = 0; j < sub_range_end; j++) {
        histo_i[j] += hptrp[j];
      }
    }
  }
}
// TODO: Multi Naive + AVX CONV
// TODO: conflict popcnt 確認せずに並列数ヒストグラム作り，最後に各要素で水平加算
