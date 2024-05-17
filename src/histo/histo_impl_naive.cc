#include "histo.h"

#include <ranges>

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
    hptr[source[i]]++;
    hptr[source[i+1]]++;
    hptr[source[i+2]]++;
    hptr[source[i+3]]++;
    hptr[source[i+4]]++;
    hptr[source[i+5]]++;
    hptr[source[i+6]]++;
    hptr[source[i+7]]++;
    hptr[source[i+8]]++;
    hptr[source[i+9]]++;
    hptr[source[i+10]]++;
    hptr[source[i+11]]++;
    hptr[source[i+12]]++;
    hptr[source[i+13]]++;
    hptr[source[i+14]]++;
    hptr[source[i+15]]++;
    hptr[source[i+16]]++;
    hptr[source[i+17]]++;
    hptr[source[i+18]]++;
    hptr[source[i+19]]++;
    hptr[source[i+20]]++;
    hptr[source[i+21]]++;
    hptr[source[i+22]]++;
    hptr[source[i+23]]++;
    hptr[source[i+24]]++;
    hptr[source[i+25]]++;
    hptr[source[i+26]]++;
    hptr[source[i+27]]++;
    hptr[source[i+28]]++;
    hptr[source[i+29]]++;
    hptr[source[i+30]]++;
    hptr[source[i+31]]++;
    // const __m512i src_v = _mm512_loadu_si512(source + i);

    // __m512i src_half  = _mm512_unpacklo_epi16(src_v, zero_v);
    // __m512i conflict  = _mm512_conflict_epi32(src_half);
    // __m512i histo_val = _mm512_i32gather_epi32(src_half, hptr, gather_scale);
    // conflict          = _mm512_popcnt_epi32(conflict);
    // histo_val         = _mm512_add_epi32(_mm512_add_epi32(histo_val, conflict), one_v);
    // _mm512_i32scatter_epi32(hptr, src_half, histo_val, gather_scale);

    // src_half  = _mm512_unpackhi_epi16(src_v, zero_v);
    // conflict  = _mm512_conflict_epi32(src_half);
    // histo_val = _mm512_i32gather_epi32(src_half, hptr, gather_scale);
    // conflict  = _mm512_popcnt_epi32(conflict);
    // histo_val = _mm512_add_epi32(_mm512_add_epi32(histo_val, conflict), one_v);
    // _mm512_i32scatter_epi32(hptr, src_half, histo_val, gather_scale);

    // // test
    // __m512i src_half = _mm512_unpacklo_epi16(src_v, zero_v);
    // __m512i conflict = _mm512_conflict_epi32(src_half);
    // // __m512i histo_val = _mm512_i32gather_epi32(src_half, hptr, gather_scale);
    // __m512i histo_val = _mm512_loadu_epi32(hptr + (i & 0xFF));
    // conflict          = _mm512_popcnt_epi32(conflict);
    // histo_val         = _mm512_add_epi32(_mm512_add_epi32(histo_val, conflict), one_v);
    // // _mm512_i32scatter_epi32(hptr, src_half, histo_val, gather_scale);
    // _mm512_storeu_epi32(hptr + (i & 0xFF), histo_val);

    // src_half = _mm512_unpackhi_epi16(src_v, zero_v);
    // conflict = _mm512_conflict_epi32(src_half);
    // // histo_val = _mm512_i32gather_epi32(src_half, hptr, gather_scale);
    // histo_val = _mm512_loadu_epi32(hptr + ((i + half_step) & 0xFF) + half_step);
    // conflict  = _mm512_popcnt_epi32(conflict);
    // histo_val = _mm512_add_epi32(_mm512_add_epi32(histo_val, conflict), one_v);
    // // _mm512_i32scatter_epi32(hptr, src_half, histo_val, gather_scale);
    // _mm512_storeu_epi32(hptr + ((i + half_step) & 0xFF), histo_val);
  }
}
