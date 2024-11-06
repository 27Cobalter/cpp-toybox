#include <chrono>
#include <cassert>
#include <format>
#include <iostream>
#include <random>
#include <ranges>
#include <numeric>

#include <inter_branch.h>

auto main() -> int32_t {
  constexpr int32_t iter       = 10 << 6;
  constexpr int32_t loop_count = 100;
  constexpr int32_t data_size  = 2048 * 2048 / 16;

  std::chrono::high_resolution_clock::time_point start, end;
  double total_int = 0;
  double total_ext = 0;

  std::random_device seed;
  std::default_random_engine gen(seed());
  std::uniform_int_distribution<> uni(0, std::numeric_limits<uint16_t>::max());

  std::shared_ptr<uint16_t[]> src_data   = std::make_shared<uint16_t[]>(data_size);
  std::shared_ptr<uint16_t[]> inter_data = std::make_shared<uint16_t[]>(data_size);
  std::shared_ptr<uint16_t[]> exter_data = std::make_shared<uint16_t[]>(data_size);

  std::shared_ptr<uint16_t[]> a1d = std::make_shared<uint16_t[]>(loop_count);
  std::shared_ptr<uint16_t[]> b1d = std::make_shared<uint16_t[]>(loop_count);
  std::shared_ptr<uint16_t[]> c1d = std::make_shared<uint16_t[]>(loop_count);
  std::shared_ptr<uint16_t[]> d1d = std::make_shared<uint16_t[]>(loop_count);
  std::shared_ptr<uint16_t[]> e1d = std::make_shared<uint16_t[]>(loop_count);
  std::shared_ptr<uint16_t[]> f1d = std::make_shared<uint16_t[]>(loop_count);
  std::shared_ptr<uint16_t[]> a2d = std::make_shared<uint16_t[]>(loop_count);
  std::shared_ptr<uint16_t[]> b2d = std::make_shared<uint16_t[]>(loop_count);
  std::shared_ptr<uint16_t[]> c2d = std::make_shared<uint16_t[]>(loop_count);
  std::shared_ptr<uint16_t[]> d2d = std::make_shared<uint16_t[]>(loop_count);
  std::shared_ptr<uint16_t[]> e2d = std::make_shared<uint16_t[]>(loop_count);
  std::shared_ptr<uint16_t[]> f2d = std::make_shared<uint16_t[]>(loop_count);

  uint16_t* sptr = src_data.get();
  uint16_t* iptr = inter_data.get();
  uint16_t* eptr = exter_data.get();

  uint16_t* a1p = a1d.get();
  uint16_t* b1p = b1d.get();
  uint16_t* c1p = c1d.get();
  uint16_t* d1p = d1d.get();
  uint16_t* e1p = e1d.get();
  uint16_t* f1p = f1d.get();
  uint16_t* a2p = a2d.get();
  uint16_t* b2p = b2d.get();
  uint16_t* c2p = c2d.get();
  uint16_t* d2p = d2d.get();
  uint16_t* e2p = e2d.get();
  uint16_t* f2p = f2d.get();

  for (auto i : std::views::iota(0, data_size)) {
    sptr[i] = uni(gen);
  }
  for (auto i : std::views::iota(0, loop_count)) {
    a1p[i] = 0;
    b1p[i] = 0;
    c1p[i] = 0;
    d1p[i] = 0;
    e1p[i] = 0;
    f1p[i] = 0;
    a2p[i] = 0;
    b2p[i] = 0;
    c2p[i] = 0;
    d2p[i] = 0;
    e2p[i] = 0;
    f2p[i] = 0;
  }

  InterBranch inter;

  std::cout << "Count Start" << std::endl;

  for (auto it : std::views::iota(0, iter)) {
    bool srli = ((it & 0b1) == 0b1);
    bool slli = ((it & 0b10) == 0b10);
    bool add  = ((it & 0b100) == 0b100);
    bool sub  = ((it & 0b1000) == 0b1000);
    bool bor  = ((it & 0b10000) == 0b10000);
    bool band = ((it & 0b100000) == 0b100000);

    start = std::chrono::high_resolution_clock::now();
    for (auto loop : std::views::iota(0, loop_count)) {
      inter.MultiOpInt(srli, slli, add, sub, bor, band, data_size, sptr, a1p[loop], b1p[loop],
                       c1p[loop], d1p[loop], e1p[loop], f1p[loop]);
    }
    end = std::chrono::high_resolution_clock::now();
    double lap_int =
        static_cast<float>(
            std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) /
        loop_count;
    if ((it & 0b111111) == 0b111111) {
      total_int += lap_int;
      std::cout << std::format("lap_int: {}, total_int: {}", lap_int, total_int) << std::endl;
    }

    start = std::chrono::high_resolution_clock::now();
    for (auto loop : std::views::iota(0, loop_count)) {
      inter.MultiOpExt(srli, slli, add, sub, bor, band, data_size, sptr, a2p[loop], b2p[loop],
                       c2p[loop], d2p[loop], e2p[loop], f2p[loop]);
    }
    end = std::chrono::high_resolution_clock::now();
    double lap_ext =
        static_cast<float>(
            std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) /
        loop_count;
    if ((it & 0b111111) == 0b111111) {
      total_ext += lap_ext;
      std::cout << std::format("lap_ext: {}, total_ext: {}", lap_ext, total_ext) << std::endl;
    }
  }

  for (auto i : std::views::iota(0, loop_count)) {
    if (a1p[i] != a2p[i])
      std::cout << std::format("[{}] a1p({}) != a2p({})", i, a1p[i], a2p[i]) << std::endl;
    if (b1p[i] != b2p[i])
      std::cout << std::format("[{}] b1p({}) != b2p({})", i, b1p[i], b2p[i]) << std::endl;
    if (c1p[i] != c2p[i])
      std::cout << std::format("[{}] c1p({}) != c2p({})", i, c1p[i], c2p[i]) << std::endl;
    if (d1p[i] != d2p[i])
      std::cout << std::format("[{}] d1p({}) != d2p({})", i, d1p[i], d2p[i]) << std::endl;
    if (e1p[i] != e2p[i])
      std::cout << std::format("[{}] e1p({}) != e2p({})", i, e1p[i], e2p[i]) << std::endl;
    if (f1p[i] != f2p[i])
      std::cout << std::format("[{}] f1p({}) != f2p({})", i, f1p[i], f2p[i]) << std::endl;
  }

  return 0;
}