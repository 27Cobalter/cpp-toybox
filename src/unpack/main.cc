#include <cstdint>
#include <memory>
#include <iostream>
#include <chrono>
#include <ranges>

#include <immintrin.h>

auto main() -> int {
  constexpr int32_t img_size      = 2048 * 2048;
  constexpr int32_t loop_num      = 10000;
  std::shared_ptr<uint8_t[]> ssp  = std::make_shared<uint8_t[]>(img_size / 2 * 3);
  std::shared_ptr<uint16_t[]> dsp = std::make_shared<uint16_t[]>(img_size);
  uint8_t* src                    = ssp.get();
  uint16_t* dst                   = dsp.get();

  {
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
          _mm256_setr_epi8(4, 5, 7, 8, 5, 6, 8, 9, 10, 11, 13, 14, 11, 12, 14, 15, 0, 1, 3, 4,
                           1, 2, 4, 5, 6, 7, 9, 10, 7, 8, 10, 11);
      const __m256i shift_v  = _mm256_set_epi32(4, 0, 4, 0, 4, 0, 4, 0);
      const __m256i and_mask = _mm256_set1_epi16(0x0FFF);
      const __m256i shuffle_idx2 =
          _mm256_setr_epi8(0, 1, 4, 5, 2, 3, 6, 7, 8, 9, 12, 13, 10, 11, 14, 15, 0, 1, 4, 5, 2,
                           3, 6, 7, 8, 9, 12, 13, 10, 11, 14, 15);
      int32_t si        = 0;
      int32_t di        = 0;
      int32_t end_count = img_size / 2 * 3;
      for (; si < end_count; si += 24, di += 16) {
        __m256i a = _mm256_loadu_epi8(src - 4 + si);
        __m256i b = _mm256_shuffle_epi8(a, shuffle_idx);
        __m256i c = _mm256_srlv_epi32(b, shift_v);
        __m256i d = _mm256_and_epi32(c, and_mask);
        __m256i e = _mm256_shuffle_epi8(d, shuffle_idx2);
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