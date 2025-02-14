#pragma once

#include <cstdint>

#include <opencv4/opencv2/core/types.hpp>
#include <opencv4/opencv2/core/core.hpp>

enum class Impl { Naive, SeqRead, Avx512 };

template <Impl IMPL>
class Binning {
public:
  template <uint32_t BINNING_X, uint32_t BINNING_Y>
  static void Execute(const cv::Mat& src, cv::Mat& dst);
};
