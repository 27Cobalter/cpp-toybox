#pragma once

#include <cstdint>
#include <print>

#include <immintrin.h>
#include <opencv4/opencv2/core/types.hpp>

enum class Impl {
  Naive,
  SeqRead,
  Avx512,
  Avx512UnrollAll,
  Avx512UnrollX,
  Avx512Seq,
  Avx512SeqBuffer
};

class BinningBase {
public:
  virtual void Execute(const cv::Mat& src, cv::Mat& dst, uint32_t binning_x,
                       uint32_t binning_y) = 0;
};

template <Impl IMPL>
class Binning : public BinningBase {
private:
  template <uint32_t... params>
  inline void Execute_Impl(uint32_t head, auto&&... args);
  template <uint32_t BINNING_X, uint32_t BINNING_Y>
  void Execute_Impl(const cv::Mat& src, cv::Mat& dst);

public:
  static const Impl impl = IMPL;

public:
  void Execute(const cv::Mat& src, cv::Mat& dst, uint32_t binning_x,
               uint32_t binning_y) override;
};

template <Impl IMPL>
template <uint32_t... params>
inline void Binning<IMPL>::Execute_Impl(uint32_t head, auto&&... args) {
  switch (head) {
    case (1):
      Execute_Impl<params..., 1>(std::forward<decltype(args)>(args)...);
      break;
    case (2):
      Execute_Impl<params..., 2>(std::forward<decltype(args)>(args)...);
      break;
    case (4):
      Execute_Impl<params..., 4>(std::forward<decltype(args)>(args)...);
      break;
    default:
      assert(false);
  }
}

inline void Print(__m512i vec) {
  std::vector<uint16_t> a(32);
  _mm512_storeu_si512(a.data(), vec);
  std::print("[");
  for (auto elem : a) std::print("{:5d},", elem);
  std::println("]");
}
