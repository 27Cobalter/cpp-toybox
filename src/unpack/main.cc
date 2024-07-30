#include <cstdint>
#include <memory>
#include <iostream>
#include <chrono>
#include <ranges>

#include <immintrin.h>

auto main() -> int {
  constexpr int32_t img_size      = 2048 * 2048;
  constexpr int32_t loop_num      = 10000;
  // constexpr int32_t loop_num      = 1;
  std::shared_ptr<uint8_t[]> ssp  = std::make_shared<uint8_t[]>(img_size / 2 * 3);
  std::shared_ptr<uint16_t[]> dsp = std::make_shared<uint16_t[]>(img_size);
  uint8_t* src                    = ssp.get();
  uint16_t* dst                   = dsp.get();

  { // mono12p
    for (int i = 0; i < img_size / 2 * 3; i += 3) {
      uint16_t elem1 = 4000 + (2 * i / 3);
      uint16_t elem2 = 4000 + (2 * i / 3) + 1;
      src[i]         = elem1 & 0xFF;
      src[i + 1]     = (elem2 & 0xF) << 4 | (elem1 & 0xF00) >> 8;
      src[i + 2]     = (elem2 & 0xFF0) >> 4;
    }

    auto start = std::chrono::high_resolution_clock::now();
    for (auto loop : std::views::iota(0, loop_num)) {
      const __m256i shuffle_idx =
          _mm256_setr_epi8(4, 5, 5, 6, 7, 8, 8, 9, 10, 11, 11, 12, 13, 14, 14, 15, 0, 1, 1, 2,
                           3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 10, 11);
      const __m256i and_mask = _mm256_set1_epi16(0x0FFF);
      int32_t si        = 0;
      int32_t di        = 0;
      int32_t end_count = img_size / 2 * 3;
      for (; si < end_count; si += 24, di += 16) {
        __m256i a = _mm256_loadu_epi8(src - 4 + si);
        __m256i b = _mm256_shuffle_epi8(a, shuffle_idx);
        __m256i c = _mm256_srli_epi32(b, 4);
        __m256i d = _mm256_blend_epi16(b, c, 0b10101010);
        __m256i e = _mm256_and_si256(d, and_mask);
        _mm256_storeu_epi16(dst + di, e);
      }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                     loop_num
              << std::endl;
  }

  { // mono12
    for (int i = 0; i < img_size / 2 * 3; i += 3) {
      uint16_t elem1 = 4000 + (2 * i / 3);
      uint16_t elem2 = 4000 + (2 * i / 3) + 1;
      src[i]         = (elem1 & 0xFF0) >> 4;
      src[i + 1]     = (elem2 & 0x00F) << 4 | (elem1 & 0x00F);
      src[i + 2]     = (elem2 & 0xFF0) >> 4;
    }

    auto start = std::chrono::high_resolution_clock::now();
    for (auto loop : std::views::iota(0, loop_num)) {
      const __m256i shuffle_idx =
          _mm256_setr_epi8(5, 4, 5, 6, 8, 7, 8,9, 11, 10, 11, 12, 14, 13, 14, 15, 1, 0, 1, 2, 4 ,3, 4, 5, 7, 6 ,7, 8, 10, 9, 10, 11);
      const __m256i blend_mask = _mm256_set1_epi32(0x000000FF);
      int32_t si        = 0;
      int32_t di        = 0;
      int32_t end_count = img_size / 2 * 3;
      for (; si < end_count; si += 24, di += 16) {
        __m256i a = _mm256_loadu_epi8(src - 4 + si);
        __m256i b = _mm256_shuffle_epi8(a, shuffle_idx);
        __m256i c = _mm256_slli_epi16(b, 4);
        __m256i d = _mm256_blendv_epi8(b, c, blend_mask);
        __m256i e = _mm256_srli_epi16(d, 4);
        _mm256_storeu_epi16(dst + di, e);
      }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                     loop_num
              << std::endl;
  }

  return 0;
}