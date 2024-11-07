#include <chrono>
#include <cstdint>
#include <format>
#include <iostream>
#include <memory>
#include <random>
#include <ranges>

// #define __AVX2__
#define __SSE42__
#if defined(__AVX2__)
#include <immintrin.h>
#elif defined(__SSE42__)
#include <smmintrin.h>
#elif defined(__ARM_NEON)
#include <arm_neon.h>
#else
static_assert(false);
#endif

auto main() -> int {
  constexpr int32_t img_size      = 2048 * 2048;
  constexpr int32_t loop_num      = 1000;
  // constexpr int32_t loop_num      = 1;
  std::shared_ptr<uint8_t[]> ssp  = std::make_shared<uint8_t[]>(img_size / 2 * 3);
  std::shared_ptr<uint16_t[]> dsp1 = std::make_shared<uint16_t[]>(img_size);
  std::shared_ptr<uint16_t[]> dsp2 = std::make_shared<uint16_t[]>(img_size);
  std::shared_ptr<uint16_t[]> dsp3 = std::make_shared<uint16_t[]>(img_size);
  std::shared_ptr<uint16_t[]> dsp4 = std::make_shared<uint16_t[]>(img_size);
  std::shared_ptr<uint16_t[]> rsp = std::make_shared<uint16_t[]>(img_size);
  uint8_t* src                    = ssp.get();
  uint16_t* dst1                   = dsp1.get();
  uint16_t* dst2                   = dsp2.get();
  uint16_t* dst3                   = dsp3.get();
  uint16_t* dst4                   = dsp4.get();
  uint16_t* ref                   = rsp.get();

  std::random_device seed;
  std::mt19937 gen(seed());

  { // mono12p
    for (int i = 0; i < img_size / 2 * 3; i += 3) {
      // uint16_t elem1 = (4000 + (2 * i / 3)) & 0x0FFF;
      // uint16_t elem2 =(4000 + (2 * i / 3) + 1) & 0x0FFF;
      uint16_t elem1 = static_cast<uint16_t>(gen()) & 0x0FFF;
      uint16_t elem2 = static_cast<uint16_t>(gen()) & 0x0FFF;
      rsp[(2 * i /3)]  =  elem1;
      ref[(2 * i /3)+1] = elem2;
      src[i]         = elem1 & 0xFF;
      src[i + 1]     = (elem2 & 0xF) << 4 | (elem1 & 0xF00) >> 8;
      src[i + 2]     = (elem2 & 0xFF0) >> 4;
    }

    { // Naive
      auto start = std::chrono::high_resolution_clock::now();
      for (auto loop : std::views::iota(0, loop_num)) {
        int32_t si        = 0;
        int32_t di        = 0;
        int32_t end_count = img_size / 2 * 3;
        for (; si < end_count; si += 3, di += 2) {
          uint8_t* srci  = src + si;
          uint16_t* dsti  = dst1 + di;
          dsti[0] = ((srci[1] & 0x0F) << 8) | srci[0];
          dsti[1] = ((srci[1] & 0xF0) >> 4) | (srci[2] << 4);
        }
      }
      auto end = std::chrono::high_resolution_clock::now();
      std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
        loop_num
        << std::endl;
    }

    { // SIMD
      auto start = std::chrono::high_resolution_clock::now();
      for (auto loop : std::views::iota(0, loop_num)) {
        int32_t si        = 0;
        int32_t di        = 0;
        int32_t end_count = img_size / 2 * 3;
#if defined(__AVX2__)
        const __m256i shuffle_idx =
          _mm256_setr_epi8(4, 5, 5, 6, 7, 8, 8, 9, 10, 11, 11, 12, 13, 14, 14, 15, 0, 1, 1, 2,
              3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 10, 11);
        const __m256i and_mask = _mm256_set1_epi16(0x0FFF);
        for (; si < end_count; si += 24, di += 16) {
          __m256i a = _mm256_loadu_epi8(src - 4 + si);
          __m256i b = _mm256_shuffle_epi8(a, shuffle_idx);
          __m256i c = _mm256_srli_epi32(b, 4);
          __m256i d = _mm256_blend_epi16(b, c, 0b10101010);
          __m256i e = _mm256_and_si256(d, and_mask);
          _mm256_storeu_epi16(dst2 + di, e);
        }
#elif defined(__SSE42__)
        const __m128i shuffle_idx =
          _mm_setr_epi8(0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 10, 11);
        const __m128i and_mask = _mm_set1_epi16(0x0FFF);
        for (; si < end_count; si += 12, di += 8) {
          __m128i a = _mm_loadu_si128(reinterpret_cast<__m128i*>(src + si));
          __m128i b = _mm_shuffle_epi8(a, shuffle_idx);
          __m128i c = _mm_srli_epi32(b, 4);
          __m128i d = _mm_blend_epi16(b, c, 0b10101010);
          __m128i e = _mm_and_si128(d, and_mask);
          _mm_storeu_si128(reinterpret_cast<__m128i*>(dst2 + di), e);
        }
#elif defined(__ARM_NEON)
        uint16x8_t and_mask =  vdupq_n_u16(0x0FFF);
        for(;si <end_count; si += 48,di+=32){
          uint8x16x3_t src_v = vld3q_u8(src+si);
          uint8x16x2_t lo8 = vzipq_u8(src_v.val[0], src_v.val[1]);
          uint8x16x2_t hi8 = vzipq_u8(src_v.val[1], src_v.val[2]);
          uint16x8x2_t dst_lo, dst_hi;
          dst_lo.val[0] = vandq_u16(vreinterpretq_u16_u8(lo8.val[0]), and_mask);
          dst_lo.val[1] = vshrq_n_u16(vreinterpretq_u16_u8(hi8.val[0]), 4);
          dst_hi.val[0] = vandq_u16(vreinterpretq_u16_u8(lo8.val[1]), and_mask);
          dst_hi.val[1] = vshrq_n_u16(vreinterpretq_u16_u8(hi8.val[1]), 4);

          vst2q_u16(dst2+di, dst_lo);
          vst2q_u16(dst2+di + 16, dst_hi);
        }
#endif
      }
      auto end = std::chrono::high_resolution_clock::now();
      std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
        loop_num
        << std::endl;
    }
  }

  { // mono12
    for (int i = 0; i < img_size / 2 * 3; i += 3) {
      uint16_t elem1 = ref[(2 * i / 3)];
      uint16_t elem2 = ref[(2 * i / 3) + 1];
      src[i]         = (elem1 & 0xFF0) >> 4;
      src[i + 1]     = (elem2 & 0x00F) << 4 | (elem1 & 0x00F);
      src[i + 2]     = (elem2 & 0xFF0) >> 4;
    }

    { // Naive
      auto start = std::chrono::high_resolution_clock::now();
      for (auto loop : std::views::iota(0, loop_num)) {
        int32_t si        = 0;
        int32_t di        = 0;
        int32_t end_count = img_size / 2 * 3;
        for (; si < end_count; si += 3, di += 2) {
          uint8_t* srci  = src + si;
          uint16_t* dsti  = dst3 + di;
          dsti[0] = (srci[0] << 4) | (srci[1] & 0x00F);
          dsti[1] = (srci[2] << 4) | (srci[1] >> 4);
        }
      }
      auto end = std::chrono::high_resolution_clock::now();
      std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
        loop_num
        << std::endl;
    }

    { // SIMD
      auto start = std::chrono::high_resolution_clock::now();
      for (auto loop : std::views::iota(0, loop_num)) {
        int32_t si        = 0;
        int32_t di        = 0;
        int32_t end_count = img_size / 2 * 3;
#if defined(__AVX2__)
        const __m256i shuffle_idx =
          _mm256_setr_epi8(5, 4, 5, 6, 8, 7, 8,9, 11, 10, 11, 12, 14, 13, 14, 15, 1, 0, 1, 2, 4 ,3, 4, 5, 7, 6 ,7, 8, 10, 9, 10, 11);
        const __m256i blend_mask = _mm256_set1_epi32(0x000000FF);
        for (; si < end_count; si += 24, di += 16) {
          __m256i a = _mm256_loadu_epi8(src - 4 + si);
          __m256i b = _mm256_shuffle_epi8(a, shuffle_idx);
          __m256i c = _mm256_slli_epi16(b, 4);
          __m256i d = _mm256_blendv_epi8(b, c, blend_mask);
          __m256i e = _mm256_srli_epi16(d, 4);
          _mm256_storeu_epi16(dst4 + di, e);
        }
#elif defined(__SSE42__)
        const __m128i shuffle_idx =
          _mm_setr_epi8(1, 0, 1, 2, 4 ,3, 4, 5, 7, 6 ,7, 8, 10, 9, 10, 11);
        const __m128i blend_mask = _mm_set1_epi32(0x000000FF);
        for (; si < end_count; si += 12, di += 8) {
          __m128i a = _mm_loadu_si128(reinterpret_cast<__m128i*>(src + si));
          __m128i b = _mm_shuffle_epi8(a, shuffle_idx);
          __m128i c = _mm_slli_epi16(b, 4);
          __m128i d = _mm_blendv_epi8(b, c, blend_mask);
          __m128i e = _mm_srli_epi16(d, 4);
          _mm_storeu_si128(reinterpret_cast<__m128i*>(dst4 + di), e);
        }
#elif defined(__ARM_NEON)
        for(; si < end_count; si += 48, di+=32){
          uint8x16x3_t src_v = vld3q_u8(src + si);
          uint8x16_t tmp_val1 = vshlq_n_u8(src_v.val[1], 4);
          uint8x16x2_t lo8 = vzipq_u8(tmp_val1, src_v.val[0]);
          uint8x16x2_t hi8 = vzipq_u8(src_v.val[1], src_v.val[2]);
          uint16x8x2_t dst_lo, dst_hi;
          dst_lo.val[0] = vshrq_n_u16(vreinterpretq_u16_u8(lo8.val[0]), 4);
          dst_lo.val[1] = vshrq_n_u16(vreinterpretq_u16_u8(hi8.val[0]), 4);
          dst_hi.val[0] = vshrq_n_u16(vreinterpretq_u16_u8(lo8.val[1]), 4);
          dst_hi.val[1] = vshrq_n_u16(vreinterpretq_u16_u8(hi8.val[1]), 4);
          vst2q_u16(dst4 + di, dst_lo);
          vst2q_u16(dst4 + di + 16, dst_hi);
        }
#endif
      }
      auto end = std::chrono::high_resolution_clock::now();
      std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
        loop_num
        << std::endl;
    }
  }

  for (auto i : std::views::iota(0, img_size)) {
    if (ref[i] != dst1[i] || ref[i] != dst2[i] || ref[i] != dst3[i] || ref[i] !=  dst4[i])
      std::cout << std::format("error dst[{}]=({}, {}, {}, {}, {})", i, ref[i],  dst1[i],  dst2[i], dst3[i], dst4[i]) << std::endl;
  }

  return 0;
}
