#include <ranges>

#include <gtest/gtest.h>
#include <opencv2/core/core.hpp>

#include <vhadd.h>
#include "misc.h"

TEST(TestVhaddAvx2, Full2048x2048) {
  constexpr int32_t width  = 2048;
  constexpr int32_t height = 2048;
  cv::Mat src              = cv::Mat(cv::Size(width, height), CV_16UC1);
  CreateTestData(src);
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(width, height);

  constexpr int32_t offset_x = 0;
  constexpr int32_t offset_y = 0;
  constexpr float horizontal = width;
  constexpr float vertical   = height;

  { // CalcV Naive
    auto result = profiler.CalcV_Impl<VHAdd::Method::AVX2>(sptr, width * height, offset_x,
                                                           offset_y, horizontal, vertical);
    ASSERT_EQ(result.size(), static_cast<int32_t>(horizontal));
    for (auto [i, elem] : std::views::enumerate(result)) {
      EXPECT_EQ(elem, static_cast<int32_t>(i + 1 + offset_x));
    }
  }
  { // CalcH Naive
    auto result = profiler.CalcH_Impl<VHAdd::Method::AVX2>(sptr, width * height, offset_x,
                                                           offset_y, horizontal, vertical);
    ASSERT_EQ(result.size(), static_cast<int32_t>(vertical));
    for (auto [i, elem] : std::views::enumerate(result)) {
      EXPECT_EQ(elem, static_cast<int32_t>(offset_x + (vertical / 2)));
    }
  }
  { // CalcVH Naive
    auto [result_v, result_h] = profiler.CalcVH_Impl<VHAdd::Method::AVX2>(
        sptr, width * height, offset_x, offset_y, horizontal, vertical);
    ASSERT_EQ(result_v.size(), static_cast<int32_t>(horizontal));
    ASSERT_EQ(result_h.size(), static_cast<int32_t>(vertical));
    for (auto [i, elem] : std::views::enumerate(result_v)) {
      EXPECT_EQ(elem, i + 1 + offset_x);
    }
    for (auto [i, elem] : std::views::enumerate(result_h)) {
      EXPECT_EQ(elem, static_cast<int32_t>(offset_x + (vertical / 2)));
    }
  }
}

TEST(TestVhaddAvx2, Half2048x2048) {
  constexpr int32_t width  = 2048;
  constexpr int32_t height = 2048;
  cv::Mat src              = cv::Mat(cv::Size(width, height), CV_16UC1);
  CreateTestData(src);
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(width, height);

  constexpr int32_t offset_x = 0;
  constexpr int32_t offset_y = 0;
  constexpr float horizontal = width / 2;
  constexpr float vertical   = height / 2;

  { // CalcV Naive
    auto result = profiler.CalcV_Impl<VHAdd::Method::AVX2>(sptr, width * height, offset_x,
                                                           offset_y, horizontal, vertical);
    ASSERT_EQ(result.size(), static_cast<int32_t>(horizontal));
    for (auto [i, elem] : std::views::enumerate(result)) {
      EXPECT_EQ(elem, static_cast<int32_t>(i + 1 + offset_x));
    }
  }
  { // CalcH Naive
    auto result = profiler.CalcH_Impl<VHAdd::Method::AVX2>(sptr, width * height, offset_x,
                                                           offset_y, horizontal, vertical);
    ASSERT_EQ(result.size(), static_cast<int32_t>(vertical));
    for (auto [i, elem] : std::views::enumerate(result)) {
      EXPECT_EQ(elem, static_cast<int32_t>(offset_x + (vertical / 2)));
    }
  }
  { // CalcVH Naive
    auto [result_v, result_h] = profiler.CalcVH_Impl<VHAdd::Method::AVX2>(
        sptr, width * height, offset_x, offset_y, horizontal, vertical);
    ASSERT_EQ(result_v.size(), static_cast<int32_t>(horizontal));
    ASSERT_EQ(result_h.size(), static_cast<int32_t>(vertical));
    for (auto [i, elem] : std::views::enumerate(result_v)) {
      EXPECT_EQ(elem, i + 1 + offset_x);
    }
    for (auto [i, elem] : std::views::enumerate(result_h)) {
      EXPECT_EQ(elem, static_cast<int32_t>(offset_x + (vertical / 2)));
    }
  }
}

TEST(TestVhaddAvx2, HalfOffset2048x2048) {
  constexpr int32_t width  = 2048;
  constexpr int32_t height = 2048;
  cv::Mat src              = cv::Mat(cv::Size(width, height), CV_16UC1);
  CreateTestData(src);
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(width, height);

  constexpr int32_t offset_x = width >> 1;
  constexpr int32_t offset_y = height >> 1;
  constexpr float horizontal = width / 2;
  constexpr float vertical   = height / 2;

  { // CalcV Naive
    auto result = profiler.CalcV_Impl<VHAdd::Method::AVX2>(sptr, width * height, offset_x,
                                                           offset_y, horizontal, vertical);
    ASSERT_EQ(result.size(), static_cast<int32_t>(horizontal));
    for (auto [i, elem] : std::views::enumerate(result)) {
      EXPECT_EQ(elem, static_cast<int32_t>(i + 1 + offset_x));
    }
  }
  { // CalcH Naive
    auto result = profiler.CalcH_Impl<VHAdd::Method::AVX2>(sptr, width * height, offset_x,
                                                           offset_y, horizontal, vertical);
    ASSERT_EQ(result.size(), static_cast<int32_t>(vertical));
    for (auto [i, elem] : std::views::enumerate(result)) {
      EXPECT_EQ(elem, static_cast<int32_t>(offset_x + (vertical / 2)));
    }
  }
  { // CalcVH Naive
    auto [result_v, result_h] = profiler.CalcVH_Impl<VHAdd::Method::AVX2>(
        sptr, width * height, offset_x, offset_y, horizontal, vertical);
    ASSERT_EQ(result_v.size(), static_cast<int32_t>(horizontal));
    ASSERT_EQ(result_h.size(), static_cast<int32_t>(vertical));
    for (auto [i, elem] : std::views::enumerate(result_v)) {
      EXPECT_EQ(elem, i + 1 + offset_x);
    }
    for (auto [i, elem] : std::views::enumerate(result_h)) {
      EXPECT_EQ(elem, static_cast<int32_t>(offset_x + (vertical / 2)));
    }
  }
}

TEST(TestVhaddAvx2, HalfOffset3rdQuad2048x2048) {
  constexpr int32_t width  = 2048;
  constexpr int32_t height = 2048;
  cv::Mat src              = cv::Mat(cv::Size(width, height), CV_16UC1);
  CreateTestData(src);
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(width, height);

  constexpr int32_t offset_x = 0;
  constexpr int32_t offset_y = height >> 1;
  constexpr float horizontal = width / 2;
  constexpr float vertical   = height / 2;

  { // CalcV Naive
    auto result = profiler.CalcV_Impl<VHAdd::Method::AVX2>(sptr, width * height, offset_x,
                                                           offset_y, horizontal, vertical);
    ASSERT_EQ(result.size(), static_cast<int32_t>(horizontal));
    for (auto [i, elem] : std::views::enumerate(result)) {
      EXPECT_EQ(elem, static_cast<int32_t>(i + 1 + offset_x));
    }
  }
  { // CalcH Naive
    auto result = profiler.CalcH_Impl<VHAdd::Method::AVX2>(sptr, width * height, offset_x,
                                                           offset_y, horizontal, vertical);
    ASSERT_EQ(result.size(), static_cast<int32_t>(vertical));
    for (auto [i, elem] : std::views::enumerate(result)) {
      EXPECT_EQ(elem, static_cast<int32_t>(offset_x + (vertical / 2)));
    }
  }
  { // CalcVH Naive
    auto [result_v, result_h] = profiler.CalcVH_Impl<VHAdd::Method::AVX2>(
        sptr, width * height, offset_x, offset_y, horizontal, vertical);
    ASSERT_EQ(result_v.size(), static_cast<int32_t>(horizontal));
    ASSERT_EQ(result_h.size(), static_cast<int32_t>(vertical));
    for (auto [i, elem] : std::views::enumerate(result_v)) {
      EXPECT_EQ(elem, i + 1 + offset_x);
    }
    for (auto [i, elem] : std::views::enumerate(result_h)) {
      EXPECT_EQ(elem, static_cast<int32_t>(offset_x + (vertical / 2)));
    }
  }
}

TEST(TestVhaddAvx2, HalfOffset333x333) {
  constexpr int32_t width  = 333;
  constexpr int32_t height = 333;
  cv::Mat src              = cv::Mat(cv::Size(width, height), CV_16UC1);
  CreateTestData(src);
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(width, height);

  constexpr int32_t offset_x = width / 2;
  constexpr int32_t offset_y = height / 2;
  constexpr float horizontal = static_cast<float>(width) / 2;
  constexpr float vertical   = static_cast<float>(height) / 2;

  { // CalcV Naive
    auto result = profiler.CalcV_Impl<VHAdd::Method::AVX2>(sptr, width * height, offset_x,
                                                           offset_y, horizontal, vertical);
    ASSERT_EQ(result.size(), static_cast<int32_t>(horizontal));
    for (auto [i, elem] : std::views::enumerate(result)) {
      EXPECT_EQ(elem, i + 1 + offset_x);
    }
  }
  { // CalcH Naive
    auto result = profiler.CalcH_Impl<VHAdd::Method::AVX2>(sptr, width * height, offset_x,
                                                           offset_y, horizontal, vertical);
    ASSERT_EQ(result.size(), static_cast<int32_t>(vertical));
    for (auto [i, elem] : std::views::enumerate(result)) {
      EXPECT_EQ(elem, static_cast<int32_t>(offset_x + (vertical / 2)));
    }
  }
  { // CalcVH Naive
    auto [result_v, result_h] = profiler.CalcVH_Impl<VHAdd::Method::AVX2>(
        sptr, width * height, offset_x, offset_y, horizontal, vertical);
    ASSERT_EQ(result_v.size(), static_cast<int32_t>(horizontal));
    ASSERT_EQ(result_h.size(), static_cast<int32_t>(vertical));
    for (auto [i, elem] : std::views::enumerate(result_v)) {
      EXPECT_EQ(elem, i + 1 + offset_x);
    }
    for (auto [i, elem] : std::views::enumerate(result_h)) {
      EXPECT_EQ(elem, static_cast<int32_t>(offset_x + (vertical / 2)));
    }
  }
}

TEST(TestVhaddAvx2, HalfOffset3rdQuad333x333) {
  constexpr int32_t width  = 333;
  constexpr int32_t height = 333;
  cv::Mat src              = cv::Mat(cv::Size(width, height), CV_16UC1);
  CreateTestData(src);
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(width, height);

  constexpr int32_t offset_x = 0;
  constexpr int32_t offset_y = height / 2;
  constexpr float horizontal = width / 2;
  constexpr float vertical   = height / 2;

  { // CalcV Naive
    auto result = profiler.CalcV_Impl<VHAdd::Method::AVX2>(sptr, width * height, offset_x,
                                                           offset_y, horizontal, vertical);
    ASSERT_EQ(result.size(), static_cast<int32_t>(horizontal));
    for (auto [i, elem] : std::views::enumerate(result)) {
      EXPECT_EQ(elem, static_cast<int32_t>(i + 1 + offset_x));
    }
  }
  { // CalcH Naive
    auto result = profiler.CalcH_Impl<VHAdd::Method::AVX2>(sptr, width * height, offset_x,
                                                           offset_y, horizontal, vertical);
    ASSERT_EQ(result.size(), static_cast<int32_t>(vertical));
    for (auto [i, elem] : std::views::enumerate(result)) {
      EXPECT_EQ(elem, static_cast<int32_t>(offset_x + (vertical / 2)));
    }
  }
  { // CalcVH Naive
    auto [result_v, result_h] = profiler.CalcVH_Impl<VHAdd::Method::AVX2>(
        sptr, width * height, offset_x, offset_y, horizontal, vertical);
    ASSERT_EQ(result_v.size(), static_cast<int32_t>(horizontal));
    ASSERT_EQ(result_h.size(), static_cast<int32_t>(vertical));
    for (auto [i, elem] : std::views::enumerate(result_v)) {
      EXPECT_EQ(elem, i + 1 + offset_x);
    }
    for (auto [i, elem] : std::views::enumerate(result_h)) {
      EXPECT_EQ(elem, static_cast<int32_t>(offset_x + (vertical / 2)));
    }
  }
}
