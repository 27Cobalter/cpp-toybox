#include <iostream>
#include <cstdint>
#include <vector>
#include <ranges>
#include <format>
#include <chrono>

#include <multi_frame_access.h>

int32_t main() {
  constexpr int32_t loop_count = 1000;
  int32_t width                = 2048;
  int32_t height               = 2048;
  int32_t frames               = 64;

  SingleAlloc single(width, height, frames);
  MultiAlloc multi(width, height, frames);

  std::vector<uint32_t> delta(width * height, 255);

  decltype(std::chrono::high_resolution_clock::now()) start, end;

  uint32_t* ret;

  start = std::chrono::high_resolution_clock::now();
  for (auto i : std::views::iota(0, loop_count)) {
    ret = single.Add(delta.data());
  }
  end   = std::chrono::high_resolution_clock::now();
  std::cout << std::format(
                   "single : {}",
                   std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count())
            << std::endl;
  std::cout << std::format("ret[0] {}, ret[width * height] {}", ret[0], ret[width * height -1])
            << std::endl;

  start = std::chrono::high_resolution_clock::now();
  for (auto i : std::views::iota(0, loop_count)) {
    ret = multi.Add(delta.data());
  }
  end   = std::chrono::high_resolution_clock::now();
  std::cout << std::format(
                   "multi : {}",
                   std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count())
            << std::endl;
  std::cout << std::format("ret[0] {}, ret[width * height] {}", ret[0], ret[width * height-1])
            << std::endl;

  start = std::chrono::high_resolution_clock::now();
  for (auto i : std::views::iota(0, loop_count)) {
    ret = single.Add(delta.data());
  }
  end   = std::chrono::high_resolution_clock::now();
  std::cout << std::format(
                   "single : {}",
                   std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count())
            << std::endl;
  std::cout << std::format("ret[0] {}, ret[width * height] {}", ret[0], ret[width * height -1])
            << std::endl;

  start = std::chrono::high_resolution_clock::now();
  for (auto i : std::views::iota(0, loop_count)) {
    ret = multi.Add(delta.data());
  }
  end   = std::chrono::high_resolution_clock::now();
  std::cout << std::format(
                   "multi : {}",
                   std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count())
            << std::endl;
  std::cout << std::format("ret[0] {}, ret[width * height] {}", ret[0], ret[width * height-1])
            << std::endl;
  return 0;
}