#include <algorithm>
#include <cassert>
#include <chrono>
#include <memory>
#include <numeric>
#include <print>
#include <ranges>
#include <type_traits>

#include <immintrin.h>

void aligned_sum(uint8_t* src, uint8_t* dst, size_t size, uint8_t offset) {
  assert(((uintptr_t)src % 32) == 0);
  assert(((uintptr_t)dst % 32) == 0);
  auto offset_v = _mm256_set1_epi8(offset);
  for (size_t i = 0; i < size; i += 32) {
    auto src_v = _mm256_load_si256(reinterpret_cast<__m256i*>(src + i));
    auto res_v = _mm256_add_epi8(src_v, offset_v);
    _mm256_store_si256(reinterpret_cast<__m256i*>(dst + i), res_v);
  }
}

void unaligned_sum(uint8_t* src, uint8_t* dst, size_t size, uint8_t offset) {
  assert(((uintptr_t)src % 32) != 0);
  assert(((uintptr_t)dst % 32) != 0);
  auto offset_v = _mm256_set1_epi8(offset);
  for (size_t i = 0; i < size; i += 32) {
    auto src_v = _mm256_loadu_si256(reinterpret_cast<__m256i*>(src + i));
    auto res_v = _mm256_add_epi8(src_v, offset_v);
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(dst + i), res_v);
  }
}

enum class AlignValT : std::underlying_type_t<std::align_val_t> {};

inline void* operator new(size_t size, AlignValT align) {
  return ::operator new(size, std::align_val_t(static_cast<std::underlying_type_t<std::align_val_t>>(align)));
}
inline void operator delete(void* ptr, size_t size, AlignValT align) {
  ::operator delete(ptr, size, std::align_val_t(static_cast<std::underlying_type_t<std::align_val_t>>(align)));
}

auto main() -> int32_t {
  size_t size = 1024 * 1024 * 256;
  std::shared_ptr<uint8_t[]> src_aligned =
      std::shared_ptr<uint8_t[]>(new (AlignValT(64)) uint8_t[size + 64](), std::default_delete<uint8_t[]>());
  std::shared_ptr<uint8_t[]> src_unaligned = std::make_shared<uint8_t[]>(size + 64);
  // std::shared_ptr<uint8_t[]>(new (AlignValT(64)) uint8_t[size + 64](), std::default_delete<uint8_t[]>());
  std::shared_ptr<uint8_t[]> dst_aligned =
      std::shared_ptr<uint8_t[]>(new (AlignValT(64)) uint8_t[size + 64](), std::default_delete<uint8_t[]>());
  std::shared_ptr<uint8_t[]> dst_unaligned = std::make_shared<uint8_t[]>(size + 64);
  // std::shared_ptr<uint8_t[]>(new (AlignValT(64)) uint8_t[size + 64](), std::default_delete<uint8_t[]>());

  uint8_t* src_aligned_ptr   = src_aligned.get();
  uint8_t* src_unaligned_ptr = src_unaligned.get();
  uint8_t* dst_aligned_ptr   = dst_aligned.get();
  uint8_t* dst_unaligned_ptr = dst_unaligned.get();

  std::println("src_aligned_ptr:     {}", (uintptr_t)src_aligned_ptr % 32);
  std::println("src_unaligned_ptr:   {}", (uintptr_t)src_unaligned_ptr % 32);
  std::println("dst_aligned_ptr:     {}", (uintptr_t)dst_aligned_ptr % 32);
  std::println("dst_unaligned_ptr:   {}", (uintptr_t)dst_unaligned_ptr % 32);

  constexpr int32_t iteration = 100;

  std::chrono::steady_clock::time_point start, end;
  std::vector<int32_t> duration[2];
  std::ranges::for_each(duration, [](auto& d) { d.reserve(iteration); });

  for (auto _ : std::views::iota(0, iteration)) {
    start = std::chrono::high_resolution_clock::now();
    aligned_sum(src_aligned_ptr, dst_aligned_ptr, size, 5);
    end = std::chrono::high_resolution_clock::now();
    duration[0].emplace_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());

    start = std::chrono::high_resolution_clock::now();
    unaligned_sum(src_unaligned_ptr, dst_unaligned_ptr, size, 5);
    end = std::chrono::high_resolution_clock::now();
    duration[1].emplace_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
  }
  std::println("[aligned]:   {}", std::reduce(duration[0].begin(), duration[0].end()));
  std::println("[unaligned]: {}", std::reduce(duration[1].begin(), duration[1].end()));
  return 0;
}