#pragma once

#include <cstdint>
#include <memory>
#include <span>

class MyHisto {
public:
  std::shared_ptr<int32_t[]> histo_ptr_ = nullptr;
  std::span<int32_t> histo_;
  std::span<int32_t> histo_all_;
  const int32_t parallel_size_ = 2;

  enum class Method {
    Naive,
    Naive_MultiSubloop,
    AVX512VPOPCNTDQ,
    AVX512VPOPCNTDQ_Order,
  };

public:
  template <Method m>
  void Create_Impl(uint16_t* source, int32_t data_size);
  MyHisto(int32_t range_max);
};
