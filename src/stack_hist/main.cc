#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <random>
#include <ranges>
#include <span>

class StackHist {
private:
  uint32_t hist_[256];

public:
  StackHist();
  uint32_t* Calc(uint16_t* src, int32_t data_size);
};

StackHist::StackHist() {
  std::span<uint32_t> sp(hist_, 256);
  std::ranges::fill(sp, 0);
}

uint32_t* StackHist::Calc(uint16_t* src, int32_t data_size) {
  for (auto i : std::views::iota(0, data_size)) {
    uint16_t val = src[i] >> 8;
    hist_[val]++;
  }
  return hist_;
}

class HeapHist {
private:
  std::shared_ptr<uint32_t[]> hist_data_;
  uint32_t* hist_;

public:
  HeapHist();
  uint32_t* Calc(uint16_t* src, int32_t data_size);
};

HeapHist::HeapHist() {
  hist_data_ = std::make_shared<uint32_t[]>(256);
  hist_      = hist_data_.get();
}

uint32_t* HeapHist::Calc(uint16_t* src, int32_t data_size) {
  for (auto i : std::views::iota(0, data_size)) {
    uint16_t val = src[i] >> 8;
    hist_[val]++;
  }
  return hist_;
}

auto main() -> int {
  std::random_device seed;
  std::default_random_engine engine(seed());
  std::uniform_int_distribution<> uni(0, 65535);
  constexpr int32_t data_size       = 8192 * 8192;
  std::shared_ptr<uint16_t[]> image = std::make_shared<uint16_t[]>(data_size);
  uint16_t* iptr                    = image.get();
  for (auto i : std::views::iota(0, data_size)) {
    image[i] = uni(engine);
  }
  StackHist sh{};
  HeapHist hh{};

  std::chrono::high_resolution_clock::time_point start, end;
  int32_t duration;

  start = std::chrono::high_resolution_clock::now();
  sh.Calc(iptr, data_size);
  end      = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  std::cout << std::format("Stack: {}", duration) << std::endl;

  start = std::chrono::high_resolution_clock::now();
  hh.Calc(iptr, data_size);
  end      = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  std::cout << std::format("Stack: {}", duration) << std::endl;
  return 0;
}