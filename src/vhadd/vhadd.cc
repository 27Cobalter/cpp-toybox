#include <cassert>
#include <memory>

#include "vhadd.h"

#include <InstructionInfo.h>

constexpr int32_t AVX512_ELEM_SIZE = 512 / 8 / sizeof(uint16_t);

template <typename T>
std::shared_ptr<T[]> make_aligned_array(int32_t size) {
#ifdef _MSC_VER
  return std::shared_ptr<T[]>(new T[size]);
#else
  return std::shared_ptr<T[]>(new (std::align_val_t(64)) T[size]);
#endif
}

VHAdd::VHAdd(int32_t width, int32_t height) : width_(width), height_(height) {
  assert(width < 65536);
  assert(height < 65536);
  // h_acc_ = make_aligned_array<int32_t>(height);
  v_acc_ = make_aligned_array<int32_t>(width + AVX512_ELEM_SIZE - 1);
  h_dst_ = make_aligned_array<uint16_t>(height + AVX512_ELEM_SIZE - 1);
  v_dst_ = make_aligned_array<uint16_t>(width + AVX512_ELEM_SIZE - 1);

  // haptr_ = h_acc_.get();
  vaptr_ = v_acc_.get();
  hdptr_ = h_dst_.get();
  vdptr_ = v_dst_.get();

  result_slice_[0] = std::span<uint16_t>(hdptr_, height);
  result_slice_[1] = std::span<uint16_t>(vdptr_, width);
}
