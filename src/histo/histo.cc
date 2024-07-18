#include "histo.h"

#include <cassert>
#include <memory>

MyHisto::MyHisto(int32_t range_max) {
  assert(parallel_size_ % 2 == 0);
  const int32_t alloc_size = range_max + 1;
#ifdef _MSC_VER
  histo_ptr_ = std::make_shared<int32_t[]>(alloc_size * parallel_size_);
#else
  histo_ptr_ = std::shared_ptr<int32_t[]>(new (std::align_val_t(64)) int32_t[alloc_size * parallel_size_]);
#endif
  histo_     = std::span<int32_t>(histo_ptr_.get(), range_max + 1);
  histo_all_ = std::span<int32_t>(histo_ptr_.get(), alloc_size * parallel_size_);
}
