#include <ranges>
#include <numeric>

#include <gtest/gtest.h>
#include <opencv2/core/core.hpp>

#include <vhadd.h>

constexpr int32_t test_val = std::numeric_limits<uint16_t>::max();

TEST(Overflow, CalcV_Naive) {
  constexpr int32_t size = 8192;
  cv::Mat src =
      cv::Mat::ones(cv::Size(size, size), CV_16UC1) * test_val;
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(src.cols, src.rows);

  auto result =
      profiler.CalcV_Impl<VHAdd::Method::Naive>(sptr, src.total(), 0, 0, src.cols, src.rows);

  ASSERT_EQ(result.size(), src.cols);
  for (auto elem : result) {
    ASSERT_EQ(elem, test_val);
  }
}

TEST(Overflow, CalcH_Naive) {
  constexpr int32_t size = 8192;
  cv::Mat src =
      cv::Mat::ones(cv::Size(size, size), CV_16UC1) * test_val;
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(src.cols, src.rows);

  auto result =
      profiler.CalcH_Impl<VHAdd::Method::Naive>(sptr, src.total(), 0, 0, src.cols, src.rows);

  ASSERT_EQ(result.size(), src.rows);
  for (auto elem : result) {
    ASSERT_EQ(elem, test_val);
  }
}

TEST(Overflow, CalcVH_Naive) {
  constexpr int32_t size = 8192;
  cv::Mat src =
      cv::Mat::ones(cv::Size(size, size), CV_16UC1) * test_val;
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(src.cols, src.rows);

  auto [result_v, result_h] =
      profiler.CalcVH_Impl<VHAdd::Method::Naive>(sptr, src.total(), 0, 0, src.cols, src.rows);

  ASSERT_EQ(result_v.size(), src.cols);
  ASSERT_EQ(result_h.size(), src.rows);
  for (auto elem : result_v) {
    ASSERT_EQ(elem, test_val);
  }
  for (auto elem : result_h) {
    ASSERT_EQ(elem, test_val);
  }
}

TEST(Overflow, CalcV_Avx2) {
  constexpr int32_t size = 8192;
  cv::Mat src =
      cv::Mat::ones(cv::Size(size, size), CV_16UC1) * test_val;
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(src.cols, src.rows);

  auto result =
      profiler.CalcV_Impl<VHAdd::Method::AVX2>(sptr, src.total(), 0, 0, src.cols, src.rows);

  ASSERT_EQ(result.size(), src.cols);
  for (auto elem : result) {
    ASSERT_EQ(elem, test_val);
  }
}

TEST(Overflow, CalcH_Avx2) {
  constexpr int32_t size = 8192;
  cv::Mat src =
      cv::Mat::ones(cv::Size(size, size), CV_16UC1) * test_val;
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(src.cols, src.rows);

  auto result =
      profiler.CalcH_Impl<VHAdd::Method::AVX2>(sptr, src.total(), 0, 0, src.cols, src.rows);

  ASSERT_EQ(result.size(), src.rows);
  for (auto elem : result) {
    ASSERT_EQ(elem, test_val);
  }
}

TEST(Overflow, CalcVH_Avx2) {
  constexpr int32_t size = 8192;
  cv::Mat src =
      cv::Mat::ones(cv::Size(size, size), CV_16UC1) * test_val;
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(src.cols, src.rows);

  auto [result_v, result_h] =
      profiler.CalcVH_Impl<VHAdd::Method::AVX2>(sptr, src.total(), 0, 0, src.cols, src.rows);

  ASSERT_EQ(result_v.size(), src.cols);
  ASSERT_EQ(result_h.size(), src.rows);
  for (auto elem : result_v) {
    ASSERT_EQ(elem, test_val);
  }
  for (auto elem : result_h) {
    ASSERT_EQ(elem, test_val);
  }
}