#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include <vector>

class SingleAlloc {
private:
  int32_t current_;
  int32_t data_size_;
  int32_t frames_;

public:
  std::shared_ptr<uint32_t[]> accumulate_;
  std::shared_ptr<uint32_t[]> round_ptr_;
  std::vector<std::span<uint32_t>> round_buffer_;
  SingleAlloc(int32_t width, int32_t height, int32_t frames);
  uint32_t* Add(uint32_t* src);
};
class MultiAlloc {
private:
  int32_t current_;
  int32_t data_size_;
  int32_t frames_;

public:
  std::shared_ptr<uint32_t[]> accumulate_;
  std::vector<std::shared_ptr<uint32_t[]>> round_buffer_;
  MultiAlloc(int32_t width, int32_t height, int32_t frames);
  uint32_t* Add(uint32_t* src);
};
