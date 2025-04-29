#include <numeric>
#include <ranges>

#include <gtest/gtest.h>
#include <opencv2/core/core.hpp>

#include <vhadd.h>

#include <InstructionInfo.h>

class Overflow : public ::testing::Test {
protected:
  using IIIS                   = InstructionInfo::InstructionSet;
  const bool supported_avx2    = InstructionInfo::IsSupported(IIIS::AVX2);
  const bool supported_avx512f = InstructionInfo::IsSupported(IIIS::AVX512F);
};

constexpr int32_t test_val   = std::numeric_limits<uint16_t>::max();
constexpr int32_t image_size = std::numeric_limits<int16_t>::max();

TEST_F(Overflow, CalcV_Naive) {
  cv::Mat src    = cv::Mat::ones(cv::Size(image_size, image_size), CV_16UC1) * test_val;
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(src.cols, src.rows);

  auto result = profiler.CalcV_Impl<VHAdd::Method::Naive>(sptr, src.total(), 0, 0, src.cols, src.rows);

  ASSERT_EQ(result.size(), src.cols);
  for (auto elem : result) {
    ASSERT_EQ(elem, test_val);
  }
}

TEST_F(Overflow, CalcH_Naive) {
  cv::Mat src    = cv::Mat::ones(cv::Size(image_size, image_size), CV_16UC1) * test_val;
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(src.cols, src.rows);

  auto result = profiler.CalcH_Impl<VHAdd::Method::Naive>(sptr, src.total(), 0, 0, src.cols, src.rows);

  ASSERT_EQ(result.size(), src.rows);
  for (auto elem : result) {
    ASSERT_EQ(elem, test_val);
  }
}

TEST_F(Overflow, CalcVH_Naive) {
  cv::Mat src    = cv::Mat::ones(cv::Size(image_size, image_size), CV_16UC1) * test_val;
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(src.cols, src.rows);

  auto [result_v, result_h] = profiler.CalcVH_Impl<VHAdd::Method::Naive>(sptr, src.total(), 0, 0, src.cols, src.rows);

  ASSERT_EQ(result_v.size(), src.cols);
  ASSERT_EQ(result_h.size(), src.rows);
  for (auto elem : result_v) {
    ASSERT_EQ(elem, test_val);
  }
  for (auto elem : result_h) {
    ASSERT_EQ(elem, test_val);
  }
}

TEST_F(Overflow, CalcV_Avx2) {
  if (supported_avx2 == false) {
    GTEST_SKIP();
  };

  cv::Mat src    = cv::Mat::ones(cv::Size(image_size, image_size), CV_16UC1) * test_val;
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(src.cols, src.rows);

  auto result = profiler.CalcV_Impl<VHAdd::Method::AVX2>(sptr, src.total(), 0, 0, src.cols, src.rows);

  ASSERT_EQ(result.size(), src.cols);
  for (auto elem : result) {
    ASSERT_EQ(elem, test_val);
  }
}

TEST_F(Overflow, CalcH_Avx2) {
  if (supported_avx2 == false) {
    GTEST_SKIP();
  };
  cv::Mat src    = cv::Mat::ones(cv::Size(image_size, image_size), CV_16UC1) * test_val;
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(src.cols, src.rows);

  auto result = profiler.CalcH_Impl<VHAdd::Method::AVX2>(sptr, src.total(), 0, 0, src.cols, src.rows);

  ASSERT_EQ(result.size(), src.rows);
  for (auto elem : result) {
    ASSERT_EQ(elem, test_val);
  }
}

TEST_F(Overflow, CalcVH_Avx2) {
  if (supported_avx2 == false) {
    GTEST_SKIP();
  };
  cv::Mat src    = cv::Mat::ones(cv::Size(image_size, image_size), CV_16UC1) * test_val;
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(src.cols, src.rows);

  auto [result_v, result_h] = profiler.CalcVH_Impl<VHAdd::Method::AVX2>(sptr, src.total(), 0, 0, src.cols, src.rows);

  ASSERT_EQ(result_v.size(), src.cols);
  ASSERT_EQ(result_h.size(), src.rows);
  for (auto elem : result_v) {
    ASSERT_EQ(elem, test_val);
  }
  for (auto elem : result_h) {
    ASSERT_EQ(elem, test_val);
  }
}

TEST_F(Overflow, CalcV_Avx512) {
  if (supported_avx512f == false) {
    GTEST_SKIP();
  };

  cv::Mat src    = cv::Mat::ones(cv::Size(image_size, image_size), CV_16UC1) * test_val;
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(src.cols, src.rows);

  auto result = profiler.CalcV_Impl<VHAdd::Method::AVX512>(sptr, src.total(), 0, 0, src.cols, src.rows);

  ASSERT_EQ(result.size(), src.cols);
  for (auto elem : result) {
    ASSERT_EQ(elem, test_val);
  }
}

TEST_F(Overflow, CalcH_Avx512) {
  if (supported_avx512f == false) {
    GTEST_SKIP();
  };
  cv::Mat src    = cv::Mat::ones(cv::Size(image_size, image_size), CV_16UC1) * test_val;
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(src.cols, src.rows);

  auto result = profiler.CalcH_Impl<VHAdd::Method::AVX512>(sptr, src.total(), 0, 0, src.cols, src.rows);

  ASSERT_EQ(result.size(), src.rows);
  for (auto elem : result) {
    ASSERT_EQ(elem, test_val);
  }
}

TEST_F(Overflow, CalcVH_Avx512) {
  if (supported_avx512f == false) {
    GTEST_SKIP();
  };
  cv::Mat src    = cv::Mat::ones(cv::Size(image_size, image_size), CV_16UC1) * test_val;
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(src.cols, src.rows);

  auto [result_v, result_h] = profiler.CalcVH_Impl<VHAdd::Method::AVX512>(sptr, src.total(), 0, 0, src.cols, src.rows);

  ASSERT_EQ(result_v.size(), src.cols);
  ASSERT_EQ(result_h.size(), src.rows);
  for (auto elem : result_v) {
    ASSERT_EQ(elem, test_val);
  }
  for (auto elem : result_h) {
    ASSERT_EQ(elem, test_val);
  }
}