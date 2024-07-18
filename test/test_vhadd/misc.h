#pragma once

#include <ranges>

#include <opencv2/core/core.hpp>

inline void CreateTestData(cv::Mat& mat) {
  uint16_t* ptr = mat.ptr<uint16_t>(0);
  for (auto j : std::views::iota(0, mat.rows)) {
    uint16_t* ptrj = ptr + mat.cols * j;
    for (auto i : std::views::iota(0, mat.cols)) {
      ptrj[i] = i + 1;
    }
  }
}
