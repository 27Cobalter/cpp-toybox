#include <chrono>
#include <cstdint>
#include <format>
#include <iostream>
#include <memory>
#include <random>
#include <ranges>

#if __has_include(<immintrin.h>) && !defined(MAC)
#include <immintrin.h>
#ifndef __AVX2__
#define __AVX2__
#endif
#ifndef __SSE42__
#define __SSE42__
#endif
#elif __has_include(<smmintrin.h>)
#include <smmintrin.h>
#ifndef __SSE42__
#define __SSE42__
#endif
#endif

#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64) || defined(_M_ARM64EC) || \
    defined(__ARM_NEON) || defined(MAC)
#ifndef __ARM_NEON
#define __ARM_NEON
#endif
#include <arm_neon.h>
#endif

auto main() -> int {
  constexpr int32_t img_size = 2048 * 2048;
  constexpr int32_t loop_num = 1000;
  // constexpr int32_t loop_num      = 1;
  std::shared_ptr<uint8_t[]> ssp           = std::make_shared<uint8_t[]>(img_size / 2 * 3);
  std::shared_ptr<uint16_t[]> dsp_naive12p = std::make_shared<uint16_t[]>(img_size);
  std::shared_ptr<uint16_t[]> dsp_naive12  = std::make_shared<uint16_t[]>(img_size);
  std::shared_ptr<uint16_t[]> dsp_sse12p   = std::make_shared<uint16_t[]>(img_size);
  std::shared_ptr<uint16_t[]> dsp_sse12    = std::make_shared<uint16_t[]>(img_size);
  std::shared_ptr<uint16_t[]> dsp_avx12p   = std::make_shared<uint16_t[]>(img_size);
  std::shared_ptr<uint16_t[]> dsp_avx12    = std::make_shared<uint16_t[]>(img_size);
  std::shared_ptr<uint16_t[]> dsp_neon12p  = std::make_shared<uint16_t[]>(img_size);
  std::shared_ptr<uint16_t[]> dsp_neon12   = std::make_shared<uint16_t[]>(img_size);
  std::shared_ptr<uint16_t[]> rsp12p       = std::make_shared<uint16_t[]>(img_size);
  std::shared_ptr<uint16_t[]> rsp12        = std::make_shared<uint16_t[]>(img_size);
  uint8_t* src                             = ssp.get();
  uint16_t* dst_naive12p                   = dsp_naive12p.get();
  uint16_t* dst_naive12                    = dsp_naive12.get();
  uint16_t* dst_avx12p                     = dsp_avx12p.get();
  uint16_t* dst_avx12                      = dsp_avx12.get();
  uint16_t* dst_sse12p                     = dsp_sse12p.get();
  uint16_t* dst_sse12                      = dsp_sse12.get();
  uint16_t* dst_neon12p                    = dsp_neon12p.get();
  uint16_t* dst_neon12                     = dsp_neon12.get();
  uint16_t* ref12p                         = rsp12p.get();
  uint16_t* ref12                          = rsp12.get();

  std::random_device seed;
  std::mt19937 gen(seed());

  { // mono12p
    std::cout << "Mono12p" << std::endl;
    for (int i = 0; i < img_size / 2 * 3; i += 3) {
      // uint16_t elem1 = (4000 + (2 * i / 3)) & 0x0FFF;
      // uint16_t elem2 =(4000 + (2 * i / 3) + 1) & 0x0FFF;
      uint16_t elem1       = static_cast<uint16_t>(gen()) & 0x0FFF;
      uint16_t elem2       = static_cast<uint16_t>(gen()) & 0x0FFF;
      ref12p[(2 * i / 3)]     = elem1;
      ref12p[(2 * i / 3) + 1] = elem2;
      src[i]               = elem1 & 0xFF;
      src[i + 1]           = (elem2 & 0xF) << 4 | (elem1 & 0xF00) >> 8;
      src[i + 2]           = (elem2 & 0xFF0) >> 4;
    }

    { // Naive
      auto start = std::chrono::high_resolution_clock::now();
      for (auto loop : std::views::iota(0, loop_num)) {
        int32_t si        = 0;
        int32_t di        = 0;
        int32_t end_count = img_size / 2 * 3;
        for (; si < end_count; si += 3, di += 2) {
          uint8_t* srci  = src + si;
          uint16_t* dsti = dst_naive12p + di;
          dsti[0]        = ((srci[1] & 0x0F) << 8) | srci[0];
          dsti[1]        = ((srci[1] & 0xF0) >> 4) | (srci[2] << 4);
        }
      }
      auto end = std::chrono::high_resolution_clock::now();
      std::cout << "Naive : "
                << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                       loop_num
                << std::endl;
    }
#if defined(__AVX2__)
    { // AVX2 Mono12p
      auto start = std::chrono::high_resolution_clock::now();
      for (auto loop : std::views::iota(0, loop_num)) {
        int32_t si        = 0;
        int32_t di        = 0;
        int32_t end_count = img_size / 2 * 3;
        const __m256i shuffle_idx =
            _mm256_setr_epi8(4, 5, 5, 6, 7, 8, 8, 9, 10, 11, 11, 12, 13, 14, 14, 15, 0, 1, 1, 2,
                             3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 10, 11);
        const __m256i and_mask = _mm256_set1_epi16(0x0FFF);
        for (; si < end_count; si += 24, di += 16) {
          __m256i a = _mm256_loadu_si256(reinterpret_cast<__m256i*>(src - 4 + si));
          __m256i b = _mm256_shuffle_epi8(a, shuffle_idx);
          __m256i c = _mm256_srli_epi32(b, 4);
          __m256i d = _mm256_blend_epi16(b, c, 0b10101010);
          __m256i e = _mm256_and_si256(d, and_mask);
          _mm256_storeu_si256(reinterpret_cast<__m256i*>(dst_avx12p + di), e);
        }
      }
      auto end = std::chrono::high_resolution_clock::now();
      std::cout << "AVX2 : "
                << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                       loop_num
                << std::endl;
    }
#endif
#if defined(__SSE42__)
    { // SSE Mono12p
      auto start = std::chrono::high_resolution_clock::now();
      for (auto loop : std::views::iota(0, loop_num)) {
        int32_t si        = 0;
        int32_t di        = 0;
        int32_t end_count = img_size / 2 * 3;
        const __m128i shuffle_idx =
            _mm_setr_epi8(0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 10, 11);
        const __m128i and_mask = _mm_set1_epi16(0x0FFF);
        for (; si < end_count; si += 12, di += 8) {
          __m128i a = _mm_loadu_si128(reinterpret_cast<__m128i*>(src + si));
          __m128i b = _mm_shuffle_epi8(a, shuffle_idx);
          __m128i c = _mm_srli_epi32(b, 4);
          __m128i d = _mm_blend_epi16(b, c, 0b10101010);
          __m128i e = _mm_and_si128(d, and_mask);
          _mm_storeu_si128(reinterpret_cast<__m128i*>(dst_sse12p + di), e);
        }
      }
      auto end = std::chrono::high_resolution_clock::now();
      std::cout << "SSE : "
                << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                       loop_num
                << std::endl;
    }
#endif
#if defined(__ARM_NEON)
    { // NEON Mono12p
      auto start = std::chrono::high_resolution_clock::now();
      for (auto loop : std::views::iota(0, loop_num)) {
        int32_t si          = 0;
        int32_t di          = 0;
        int32_t end_count   = img_size / 2 * 3;
        uint16x8_t and_mask = vdupq_n_u16(0x0FFF);
        for (; si < end_count; si += 48, di += 32) {
          uint8x16x3_t src_v = vld3q_u8(src + si);
          uint8x16x2_t lo8   = vzipq_u8(src_v.val[0], src_v.val[1]);
          uint8x16x2_t hi8   = vzipq_u8(src_v.val[1], src_v.val[2]);
          uint16x8x2_t dst_lo, dst_hi;
          dst_lo.val[0] = vandq_u16(vreinterpretq_u16_u8(lo8.val[0]), and_mask);
          dst_lo.val[1] = vshrq_n_u16(vreinterpretq_u16_u8(hi8.val[0]), 4);
          dst_hi.val[0] = vandq_u16(vreinterpretq_u16_u8(lo8.val[1]), and_mask);
          dst_hi.val[1] = vshrq_n_u16(vreinterpretq_u16_u8(hi8.val[1]), 4);

          vst2q_u16(dst_neon12p + di, dst_lo);
          vst2q_u16(dst_neon12p + di + 16, dst_hi);
        }
      }
      auto end = std::chrono::high_resolution_clock::now();
      std::cout << "NEON : "
                << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                       loop_num
                << std::endl;
    }
#endif
  }

  { // mono12
    std::cout << "Mono12" << std::endl;
    for (int i = 0; i < img_size / 2 * 3; i += 3) {
      uint16_t elem1 = ref12p[(2 * i / 3)];
      uint16_t elem2 = ref12p[(2 * i / 3) + 1];
      ref12[(2 * i / 3)] = elem1;
      ref12[(2 * i / 3) + 1] = elem2;
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
          uint16_t* dsti = dst_naive12 + di;
          dsti[0]        = (srci[0] << 4) | (srci[1] & 0x00F);
          dsti[1]        = (srci[2] << 4) | (srci[1] >> 4);
        }
      }
      auto end = std::chrono::high_resolution_clock::now();
      std::cout << "Naive : "
                << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                       loop_num
                << std::endl;
    }

#if defined(__AVX2__)
    { // AVX2 Mono12
      auto start = std::chrono::high_resolution_clock::now();
      for (auto loop : std::views::iota(0, loop_num)) {
        int32_t si        = 0;
        int32_t di        = 0;
        int32_t end_count = img_size / 2 * 3;
        const __m256i shuffle_idx =
            _mm256_setr_epi8(5, 4, 5, 6, 8, 7, 8, 9, 11, 10, 11, 12, 14, 13, 14, 15, 1, 0, 1, 2,
                             4, 3, 4, 5, 7, 6, 7, 8, 10, 9, 10, 11);
        const __m256i blend_mask = _mm256_set1_epi32(0x000000FF);
        for (; si < end_count; si += 24, di += 16) {
          __m256i a = _mm256_loadu_si256(reinterpret_cast<__m256i*>(src - 4 + si));
          __m256i b = _mm256_shuffle_epi8(a, shuffle_idx);
          __m256i c = _mm256_slli_epi16(b, 4);
          __m256i d = _mm256_blendv_epi8(b, c, blend_mask);
          __m256i e = _mm256_srli_epi16(d, 4);
          _mm256_storeu_si256(reinterpret_cast<__m256i*>(dst_avx12 + di), e);
        }
      }
      auto end = std::chrono::high_resolution_clock::now();
      std::cout << "AVX2 : "
                << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                       loop_num
                << std::endl;
    }
#endif
#if defined(__SSE42__)
    { // SSE Mono12
      auto start = std::chrono::high_resolution_clock::now();
      for (auto loop : std::views::iota(0, loop_num)) {
        int32_t si        = 0;
        int32_t di        = 0;
        int32_t end_count = img_size / 2 * 3;
        const __m128i shuffle_idx =
            _mm_setr_epi8(1, 0, 1, 2, 4, 3, 4, 5, 7, 6, 7, 8, 10, 9, 10, 11);
        const __m128i blend_mask = _mm_set1_epi32(0x000000FF);
        for (; si < end_count; si += 12, di += 8) {
          __m128i a = _mm_loadu_si128(reinterpret_cast<__m128i*>(src + si));
          __m128i b = _mm_shuffle_epi8(a, shuffle_idx);
          __m128i c = _mm_slli_epi16(b, 4);
          __m128i d = _mm_blendv_epi8(b, c, blend_mask);
          __m128i e = _mm_srli_epi16(d, 4);
          _mm_storeu_si128(reinterpret_cast<__m128i*>(dst_sse12 + di), e);
        }
      }
      auto end = std::chrono::high_resolution_clock::now();
      std::cout << "SSE : "
                << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                       loop_num
                << std::endl;
    }
#endif
#if defined(__ARM_NEON)
    { // NEON Mono12
      auto start = std::chrono::high_resolution_clock::now();
      for (auto loop : std::views::iota(0, loop_num)) {
        int32_t si        = 0;
        int32_t di        = 0;
        int32_t end_count = img_size / 2 * 3;
        for (; si < end_count; si += 48, di += 32) {
          uint8x16x3_t src_v  = vld3q_u8(src + si);
          uint8x16_t tmp_val1 = vshlq_n_u8(src_v.val[1], 4);
          uint8x16x2_t lo8    = vzipq_u8(tmp_val1, src_v.val[0]);
          uint8x16x2_t hi8    = vzipq_u8(src_v.val[1], src_v.val[2]);
          uint16x8x2_t dst_lo, dst_hi;
          dst_lo.val[0] = vshrq_n_u16(vreinterpretq_u16_u8(lo8.val[0]), 4);
          dst_lo.val[1] = vshrq_n_u16(vreinterpretq_u16_u8(hi8.val[0]), 4);
          dst_hi.val[0] = vshrq_n_u16(vreinterpretq_u16_u8(lo8.val[1]), 4);
          dst_hi.val[1] = vshrq_n_u16(vreinterpretq_u16_u8(hi8.val[1]), 4);
          vst2q_u16(dst_neon12 + di, dst_lo);
          vst2q_u16(dst_neon12 + di + 16, dst_hi);
        }
      }
      auto end = std::chrono::high_resolution_clock::now();
      std::cout << "NEON : "
                << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                       loop_num
                << std::endl;
    }
#endif
  }

  for (auto i : std::views::iota(0, img_size)) {
    bool eol = false;
    if (ref12p[i] != dst_naive12p[i]) {
      std::cout << std::format("naive12p({}) ", dst_naive12p[i]);
      eol = true;
    }
    if (ref12p[i] != dst_naive12[i]) {
      std::cout << std::format("naive12({}) ", dst_naive12[i]);
      eol = true;
    }
#if defined(__AVX2__)
    if (ref12p[i] != dst_avx12p[i]) {
      std::cout << std::format("avx12p({}) ", dst_avx12p[i]);
      eol = true;
    }
    if (ref12p[i] != dst_avx12[i]) {
      std::cout << std::format("avx12({}) ", dst_avx12[i]);
      eol = true;
    }
#endif
#if defined(__SSE42__)
    if (ref12p[i] != dst_sse12p[i]) {
      std::cout << std::format("sse12p({}) ", dst_sse12p[i]);
      eol = true;
    }
    if (ref12p[i] != dst_sse12[i]) {
      std::cout << std::format("sse12({}) ", dst_sse12[i]);
      eol = true;
    }
#endif
#if defined __ARM_NEON
    if (ref12p[i] != dst_neon12p[i]) {
      std::cout << std::format("neon12p({}) ", dst_naive12p[i]);
      eol = true;
    }
    if (ref12p[i] != dst_neon12[i]) {
      std::cout << std::format("neon12({}) ", dst_neon12[i]);
      eol = true;
    }
#endif
    if (eol) std::cout << std::endl;
  }

  return 0;
}
