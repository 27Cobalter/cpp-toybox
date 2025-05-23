#include <ranges>

#include <gtest/gtest.h>
#include <opencv2/core/core.hpp>

#include <InstructionInfo.h>
#include <vhadd.h>

#pragma region GTestSettings
struct Params {
  int32_t width;
  int32_t height;
  int32_t offset_x;
  int32_t offset_y;
  int32_t horizontal;
  int32_t vertical;
};

auto TestValues = ::testing::Values(
    Params{.width = 2048, .height = 2048, .offset_x = 0, .offset_y = 0, .horizontal = 2048, .vertical = 2048},
    Params{.width = 2048, .height = 2048, .offset_x = 0, .offset_y = 0, .horizontal = 2048 / 2, .vertical = 2048 / 2},
    Params{.width      = 2048,
           .height     = 2048,
           .offset_x   = 2048 / 2,
           .offset_y   = 2048 / 2,
           .horizontal = 2048 / 2,
           .vertical   = 2048 / 2},
    Params{.width      = 333,
           .height     = 333,
           .offset_x   = 333 / 2,
           .offset_y   = 333 / 2,
           .horizontal = 333 / 2,
           .vertical   = 333 / 2},
    Params{.width = 335, .height = 157, .offset_x = 129, .offset_y = 23, .horizontal = 133, .vertical = 111},
    Params{.width = 9, .height = 9, .offset_x = 0, .offset_y = 0, .horizontal = 9, .vertical = 9},
    Params{.width = 2, .height = 2, .offset_x = 0, .offset_y = 0, .horizontal = 2, .vertical = 2});

class MultiParam : public ::testing::TestWithParam<Params> {
protected:
  using IIIS                   = InstructionInfo::InstructionSet;
  const bool supported_avx2    = InstructionInfo::IsSupported(IIIS::AVX2);
  const bool supported_avx512f = InstructionInfo::IsSupported(IIIS::AVX512F);
};
INSTANTIATE_TEST_CASE_P(, MultiParam, TestValues);

::std::ostream& operator<<(::std::ostream& os, const Params param) {
  return os << std::format("{{w={}, h={}, rect(x={}, y={}, h={}, v={})}}", param.width, param.height, param.offset_x,
                           param.offset_y, param.horizontal, param.vertical);
}
#pragma endregion GTestSettings

void SetTestData(cv::Mat& mat) {
  uint16_t* ptr = mat.ptr<uint16_t>(0);
  for (auto j : std::views::iota(0, mat.rows)) {
    uint16_t* ptrj = ptr + mat.cols * j;
    for (auto i : std::views::iota(0, mat.cols)) {
      ptrj[i] = i;
    }
  }
}

#pragma region CalcV
TEST_P(MultiParam, CalcV_Naive) {
  const auto param = GetParam();

  cv::Mat src = cv::Mat(cv::Size(param.width, param.height), CV_16UC1);
  SetTestData(src);
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(param.width, param.height);

  for (auto _ : std::views::iota(0, 2)) {
    auto result = profiler.CalcV_Impl<VHAdd::Method::Naive>(sptr, param.width * param.height, param.offset_x,
                                                            param.offset_y, param.horizontal, param.vertical);
    ASSERT_EQ(result.size(), param.horizontal);
    for (auto [i, elem] : std::views::enumerate(result)) {
      ASSERT_EQ(elem, i + param.offset_x);
    }
  }
}

TEST_P(MultiParam, CalcH_Naive) {
  const auto param = GetParam();

  cv::Mat src = cv::Mat(cv::Size(param.width, param.height), CV_16UC1);
  SetTestData(src);
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(param.width, param.height);

  for (auto _ : std::views::iota(0, 2)) {
    auto result = profiler.CalcH_Impl<VHAdd::Method::Naive>(sptr, param.width * param.height, param.offset_x,
                                                            param.offset_y, param.horizontal, param.vertical);
    ASSERT_EQ(result.size(), param.vertical);
    for (auto [i, elem] : std::views::enumerate(result)) {
      ASSERT_EQ(elem, param.offset_x + ((param.horizontal - 1) / 2));
    }
  }
}

TEST_P(MultiParam, CalcVH_Naive) {
  const auto param = GetParam();

  cv::Mat src = cv::Mat(cv::Size(param.width, param.height), CV_16UC1);
  SetTestData(src);
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(param.width, param.height);

  for (auto _ : std::views::iota(0, 2)) {
    auto [result_v, result_h] = profiler.CalcVH_Impl<VHAdd::Method::Naive>(
        sptr, param.width * param.height, param.offset_x, param.offset_y, param.horizontal, param.vertical);
    ASSERT_EQ(result_v.size(), param.horizontal);
    ASSERT_EQ(result_h.size(), param.vertical);
    for (auto [i, elem] : std::views::enumerate(result_v)) {
      ASSERT_EQ(elem, i + param.offset_x);
    }
    for (auto [i, elem] : std::views::enumerate(result_h)) {
      ASSERT_EQ(elem, param.offset_x + (param.horizontal - 1) / 2);
    }
  }
}
#pragma endregion CalcV

#pragma region AVX2
TEST_P(MultiParam, CalcH_Avx2) {
  if (supported_avx2 == false) GTEST_SKIP();

  const auto param = GetParam();

  cv::Mat src = cv::Mat(cv::Size(param.width, param.height), CV_16UC1);
  SetTestData(src);
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(param.width, param.height);

  for (auto _ : std::views::iota(0, 2)) {
    auto result = profiler.CalcV_Impl<VHAdd::Method::AVX2>(sptr, param.width * param.height, param.offset_x,
                                                           param.offset_y, param.horizontal, param.vertical);
    ASSERT_EQ(result.size(), param.horizontal);
    for (auto [i, elem] : std::views::enumerate(result)) {
      ASSERT_EQ(elem, i + param.offset_x);
    }
  }
}

TEST_P(MultiParam, CalcV_Avx2) {
  if (supported_avx2 == false) GTEST_SKIP();

  const auto param = GetParam();

  cv::Mat src = cv::Mat(cv::Size(param.width, param.height), CV_16UC1);
  SetTestData(src);
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(param.width, param.height);

  for (auto _ : std::views::iota(0, 2)) {
    auto result = profiler.CalcH_Impl<VHAdd::Method::AVX2>(sptr, param.width * param.height, param.offset_x,
                                                           param.offset_y, param.horizontal, param.vertical);
    ASSERT_EQ(result.size(), param.vertical);
    for (auto [i, elem] : std::views::enumerate(result)) {
      ASSERT_EQ(elem, param.offset_x + ((param.horizontal - 1) / 2));
    }
  }
}

TEST_P(MultiParam, CalcVH_Avx2) {
  if (supported_avx2 == false) GTEST_SKIP();

  const auto param = GetParam();

  cv::Mat src = cv::Mat(cv::Size(param.width, param.height), CV_16UC1);
  SetTestData(src);
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(param.width, param.height);

  for (auto _ : std::views::iota(0, 2)) {
    auto [result_v, result_h] = profiler.CalcVH_Impl<VHAdd::Method::AVX2>(
        sptr, param.width * param.height, param.offset_x, param.offset_y, param.horizontal, param.vertical);
    ASSERT_EQ(result_v.size(), param.horizontal);
    ASSERT_EQ(result_h.size(), param.vertical);
    for (auto [i, elem] : std::views::enumerate(result_v)) {
      ASSERT_EQ(elem, i + param.offset_x);
    }
    for (auto [i, elem] : std::views::enumerate(result_h)) {
      ASSERT_EQ(elem, param.offset_x + (param.horizontal - 1) / 2);
    }
  }
}
#pragma endregion AVX2

#pragma region AVX512
TEST_P(MultiParam, CalcH_Avx512) {
  if (supported_avx512f == false) GTEST_SKIP();

  const auto param = GetParam();

  cv::Mat src = cv::Mat(cv::Size(param.width, param.height), CV_16UC1);
  SetTestData(src);
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(param.width, param.height);

  for (auto _ : std::views::iota(0, 2)) {
    auto result = profiler.CalcV_Impl<VHAdd::Method::AVX512>(sptr, param.width * param.height, param.offset_x,
                                                             param.offset_y, param.horizontal, param.vertical);
    ASSERT_EQ(result.size(), param.horizontal);
    for (auto [i, elem] : std::views::enumerate(result)) {
      ASSERT_EQ(elem, i + param.offset_x);
    }
  }
}

TEST_P(MultiParam, CalcV_Avx512) {
  if (supported_avx512f == false) GTEST_SKIP();

  const auto param = GetParam();

  cv::Mat src = cv::Mat(cv::Size(param.width, param.height), CV_16UC1);
  SetTestData(src);
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(param.width, param.height);

  for (auto _ : std::views::iota(0, 2)) {
    auto result = profiler.CalcH_Impl<VHAdd::Method::AVX512>(sptr, param.width * param.height, param.offset_x,
                                                             param.offset_y, param.horizontal, param.vertical);
    ASSERT_EQ(result.size(), param.vertical);
    for (auto [i, elem] : std::views::enumerate(result)) {
      ASSERT_EQ(elem, param.offset_x + ((param.horizontal - 1) / 2));
    }
  }
}

TEST_P(MultiParam, CalcVH_Avx512) {
  if (supported_avx512f == false) GTEST_SKIP();

  const auto param = GetParam();

  cv::Mat src = cv::Mat(cv::Size(param.width, param.height), CV_16UC1);
  SetTestData(src);
  uint16_t* sptr = src.ptr<uint16_t>(0);

  VHAdd profiler(param.width, param.height);

  for (auto _ : std::views::iota(0, 2)) {
    auto [result_v, result_h] = profiler.CalcVH_Impl<VHAdd::Method::AVX512>(
        sptr, param.width * param.height, param.offset_x, param.offset_y, param.horizontal, param.vertical);
    ASSERT_EQ(result_v.size(), param.horizontal);
    ASSERT_EQ(result_h.size(), param.vertical);
    for (auto [i, elem] : std::views::enumerate(result_v)) {
      ASSERT_EQ(elem, i + param.offset_x);
    }
    for (auto [i, elem] : std::views::enumerate(result_h)) {
      ASSERT_EQ(elem, param.offset_x + (param.horizontal - 1) / 2);
    }
  }
}
#pragma endregion AVX512
