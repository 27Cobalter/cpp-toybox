#include <chrono>
#include <cstdint>
#include <format>
#include <iostream>
#include <memory>
#include <random>
#include <ranges>

#if _MSC_VER
#define NOINLINE __declspec(noinline)
#else
#define NOINLINE
#endif

NOINLINE uint16_t branch_hit() {
  return 1;
}

NOINLINE uint16_t branch_miss() {
  return 0;
}

auto main() -> int {
  constexpr int32_t size     = 4096 * 4096;
  constexpr int32_t iter_num = 10;
  constexpr int32_t loop_num = 100;

  int32_t threshold = 4000;

  std::random_device seed;
  std::mt19937 gen(seed());
  std::uniform_int_distribution<> low(0, threshold - 1);
  std::uniform_int_distribution<> high(threshold, 8191);
  std::uniform_int_distribution<> full(0, 8191);

  std::shared_ptr<uint16_t[]> hit  = std::make_shared<uint16_t[]>(size);
  std::shared_ptr<uint16_t[]> miss = std::make_shared<uint16_t[]>(size);

  uint16_t* sptr = hit.get();
  uint16_t* mptr = miss.get();
  for (auto i : std::views::iota(0, size / 2)) {
    sptr[i] = low(gen);
    mptr[i] = full(gen);
  }
  for (auto i : std::views::iota(size >> 1, size)) {
    sptr[i] = high(gen);
    mptr[i] = full(gen);
  }

  int32_t s_total= 0;
  int32_t m_total = 0;

  for (auto iter : std::views::iota(0, iter_num)) {
    int32_t sret = 0;
    int32_t mret = 0;

    std::chrono::high_resolution_clock::time_point start, end;

    start = std::chrono::high_resolution_clock::now();
    // for(int32_t loop = 0; loop < loop_num; loop++) {
    //   for(int32_t i  =  0; i < size; i++){
    //     if (sptr[i] < threshold)
    //       sret += branch_hit();
    //     else
    //       sret += branch_miss();
    //   }
    // }
    end = std::chrono::high_resolution_clock::now();
    int32_t s_lap = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    s_total += s_lap;
    std::cout << std::format("Hit: {}, sret: {}", static_cast<float>(s_lap) / loop_num, sret)
      << std::endl;

    start = std::chrono::high_resolution_clock::now();
    for(int32_t loop  =  0; loop < loop_num; loop++){
      for(int32_t i = 0; i < size; i++){
        if (mptr[i] < threshold)
          mret += branch_hit();
        else
          mret += branch_miss();
      }
    }
    end = std::chrono::high_resolution_clock::now();
    int32_t m_lap = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    m_total += m_lap;
    std::cout << std::format("Miss: {}, mret: {}", static_cast<float>(m_lap) / loop_num, mret)
      << std::endl;
    ;
  }
  std::cout << std::format("Total ({}, {}), Lap ({}, {})", s_total, m_total,
      s_total / loop_num / iter_num, m_total / loop_num / iter_num)
    << std::endl;
  ;

  return 0;
}
