#include "histo.h"

MyHisto::MyHisto(int32_t range_max) {
  histo_ptr_ = std::shared_ptr<int32_t[]>(new (std::align_val_t(64)) int32_t[range_max + 1]);
  histo_     = std::span<int32_t>(histo_ptr_.get(), range_max + 1);
}
