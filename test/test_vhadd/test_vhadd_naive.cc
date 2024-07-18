#include <ranges>

#include <gtest/gtest.h>
#include <opencv2/core/core.hpp>

#include <vhadd.h>
#include "misc.h"

TEST(TestVhaddNaive, Full2048x2048) {
  constexpr int32_t width  = 2048;
  constexpr int32_t height = 2048;
  cv::Mat src              = cv::Mat(cv::Size(width, height), CV_16UC1);
  CreateTestData(src);
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(width, height);

  { // CalcV Naive
    auto result =
        profiler.CalcV_Impl<VHAdd::Method::Naive>(sptr, width * height, 0, 0, width, height);
    ASSERT_EQ(result.size(), width);
    for (auto [i, elem] : std::views::enumerate(result)) {
      EXPECT_EQ(elem, i + 1);
    }
  }
  { // CalcH Naive
    auto result =
        profiler.CalcH_Impl<VHAdd::Method::Naive>(sptr, width * height, 0, 0, width, height);
    ASSERT_EQ(result.size(), height);
    for (auto [i, elem] : std::views::enumerate(result)) {
      EXPECT_EQ(elem, height >> 1);
    }
  }
  { // CalcVH Naive
    auto [result_v, result_h] =
        profiler.CalcVH_Impl<VHAdd::Method::Naive>(sptr, width * height, 0, 0, width, height);
    ASSERT_EQ(result_v.size(), width);
    ASSERT_EQ(result_h.size(), height);
    for (auto [i, elem] : std::views::enumerate(result_v)) {
      EXPECT_EQ(elem, i + 1);
    }
    for (auto [i, elem] : std::views::enumerate(result_h)) {
      EXPECT_EQ(elem, height >> 1);
    }
  }
}
