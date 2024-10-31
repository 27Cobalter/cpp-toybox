#include <chrono>
#include <cassert>
#include <format>
#include <iostream>
#include <random>
#include <ranges>
#include <numeric>

#include <inter_branch.h>

auto main() -> int32_t {
  constexpr int32_t iter       = 100;
  constexpr int32_t loop_count = 1000;
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

  std::shared_ptr<uint16_t[]> a1d = std::make_shared<uint16_t[]>(data_size);
  std::shared_ptr<uint16_t[]> b1d = std::make_shared<uint16_t[]>(data_size);
  std::shared_ptr<uint16_t[]> c1d = std::make_shared<uint16_t[]>(data_size);
  std::shared_ptr<uint16_t[]> d1d = std::make_shared<uint16_t[]>(data_size);
  std::shared_ptr<uint16_t[]> e1d = std::make_shared<uint16_t[]>(data_size);
  std::shared_ptr<uint16_t[]> f1d = std::make_shared<uint16_t[]>(data_size);
  std::shared_ptr<uint16_t[]> a2d = std::make_shared<uint16_t[]>(data_size);
  std::shared_ptr<uint16_t[]> b2d = std::make_shared<uint16_t[]>(data_size);
  std::shared_ptr<uint16_t[]> c2d = std::make_shared<uint16_t[]>(data_size);
  std::shared_ptr<uint16_t[]> d2d = std::make_shared<uint16_t[]>(data_size);
  std::shared_ptr<uint16_t[]> e2d = std::make_shared<uint16_t[]>(data_size);
  std::shared_ptr<uint16_t[]> f2d = std::make_shared<uint16_t[]>(data_size);

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

  InterBranch inter;

  std::cout << "Count Start" << std::endl;

  for (auto it : std::views::iota(0, iter)) {
    // bool srli = ((it & 0x1) == 0x1);
    // bool slli = ((it & 0x2) == 0x2);
    // bool add  = ((it & 0x4) == 0x4);
    // bool sub  = ((it & 0x8) == 0x8);
    // bool bor  = ((it & 0x10) == 0x10);
    // bool band = ((it & 0x20) == 0x20);
    bool srli = false;
    bool slli = true;
    bool add  = false;
    bool sub  = true;
    bool bor  = false;
    bool band = false;

    start = std::chrono::high_resolution_clock::now();
    for (auto loop : std::views::iota(0, loop_count)) {
      inter.MultiOpInt(srli, slli, add, sub, bor, band, data_size, sptr, a1p, b1p, c1p, d1p, e1p, f1p);
    }
    end = std::chrono::high_resolution_clock::now();
    double lap_int =
        static_cast<float>(
            std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) /
        loop_count;
    total_int += lap_int;
    std::cout << std::format("lap_int: {}, total_int: {}", lap_int, total_int) << std::endl;

    start = std::chrono::high_resolution_clock::now();
    for (auto loop : std::views::iota(0, loop_count)) {
      inter.MultiOpExt(srli, slli, add, sub, bor, band, data_size, sptr, a2p, b2p, c2p, d2p, e2p, f2p);
    }
    end = std::chrono::high_resolution_clock::now();
    double lap_ext =
        static_cast<float>(
            std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) /
        loop_count;
    total_ext += lap_ext;
    std::cout << std::format("lap_ext: {}, total_ext: {}", lap_ext, total_ext) << std::endl;
  }

  for (auto i : std::views::iota(0, data_size)) {
    assert(iptr[i] == eptr[i]);
  }

  return 0;
}