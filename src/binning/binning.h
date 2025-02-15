#pragma once

#include <cstdint>

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

template <Impl IMPL>
class Binning {
  template <uint32_t... params>
  inline void Execute_Impl(uint32_t head, auto&&... args);
  template <uint32_t BINNING_X, uint32_t BINNING_Y>
  void Execute_Impl(const cv::Mat& src, cv::Mat& dst);

public:
  void Execute(const cv::Mat& src, cv::Mat& dst, uint32_t binning_x, uint32_t binning_y);
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

template <Impl IMPL>
inline void Binning<IMPL>::Execute(const cv::Mat& src, cv::Mat& dst, uint32_t binning_x,
                                   uint32_t binning_y) {
  Execute_Impl(binning_x, binning_y, src, dst);
}
