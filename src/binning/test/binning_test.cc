#include <gtest/gtest.h>

#include <print>
#include <random>
#include <ranges>

#include <opencv2/core/core.hpp>

#include "binning.h"

enum class TestData { max, seq, rand };

void PrintTo(const TestData& t, std::ostream* os) {
  switch (t) {
  case (TestData::max):
    *os << "TestData::max";
    break;
  case (TestData::seq):
    *os << "TestData::seq";
    break;
  case (TestData::rand):
    *os << "TestData::rand";
    break;
  default:
    assert(false);
    *os << "Unknown";
    break;
  }
}

void PrintTo(const Impl& impl, std::ostream* os) {
  switch (impl) {
  case (Impl::None):
    *os << "Impl::None";
    break;
  case (Impl::Naive):
    *os << "Impl::Naive";
    break;
  case (Impl::SeqRead):
    *os << "Impl::SeqRead";
    break;
  case (Impl::Avx512):
    *os << "Impl::Avx512";
    break;
  case (Impl::Avx512UnrollAll):
    *os << "Impl::Avx512UnrollAll";
    break;
  case (Impl::Avx512UnrollX):
    *os << "Impl::Avx512UnrollX";
    break;
  case (Impl::Avx512UnrollLoad):
    *os << "Impl::Avx512UnrollLoad";
    break;
  case (Impl::Avx512Seq):
    *os << "Impl::Avx512Seq";
    break;
  case (Impl::Avx512SeqBuffer):
    *os << "Impl::Avx512SeqBuffer";
    break;
  default:
    assert(false);
    *os << "Unknown";
  }
}

void PrintTo(const std::shared_ptr<BinningBase>& binning, std::ostream* os) {
  PrintTo(binning->GetImpl(), os);
}

auto TESTIMPL  = ::testing::Values(std::make_shared<Binning<Impl::SeqRead>>()
                                   // std::make_shared<Binning<Impl::Avx512>>(),
                                   // std::make_shared<Binning<Impl::Avx512UnrollX>>(),
                                   // std::make_shared<Binning<Impl::Avx512UnrollLoad>>(),
                                   // std::make_shared<Binning<Impl::Avx512Seq>>(),
                                   // std::make_shared<Binning<Impl::Avx512SeqBuffer>>()
                                   );
auto BINNING_X = ::testing::Values(1, 2, 4);
auto BINNING_Y = ::testing::Values(1, 2, 4);
auto TESTDATA  = ::testing::Values(TestData::max, TestData::seq, TestData::rand);

using TestParams = std::tuple<std::shared_ptr<BinningBase>, uint32_t, uint32_t, TestData>;
class BINNING_TEST : public ::testing::TestWithParam<TestParams> {};
INSTANTIATE_TEST_CASE_P(, BINNING_TEST, ::testing::Combine(TESTIMPL, BINNING_X, BINNING_Y, TESTDATA));

constexpr int32_t width  = 4096;
constexpr int32_t height = 4096;

cv::Mat CreateTestData(TestData pattern) {
  cv::Size size(width, height);
  if (pattern == TestData::max) {
    return cv::Mat::ones(size, CV_16UC1) * std::numeric_limits<uint16_t>::max();
  } else if (pattern == TestData::seq) {
    cv::Mat data = cv::Mat::zeros(cv::Size(width, height), CV_16UC1);
    for (auto y : std::views::iota(0, data.rows)) {
      uint16_t* sptry = data.ptr<uint16_t>(y);
      for (auto x : std::views::iota(0, data.cols)) {
        sptry[x] = y + x;
      }
    }
    return data;
  } else if (pattern == TestData::rand) {
    std::random_device seed;
    std::mt19937 mt(seed());
    cv::Mat data = cv::Mat::zeros(cv::Size(width, height), CV_16UC1);
    for (auto y : std::views::iota(0, data.rows)) {
      uint16_t* sptry = data.ptr<uint16_t>(y);
      for (auto x : std::views::iota(0, data.cols)) {
        sptry[x] = mt() & 0xFFFF;
      }
    }
    return data;
  } else {
    assert(false);
    return cv::Mat::zeros(size, CV_16UC1);
  }
}

TEST_P(BINNING_TEST, Normal) {
  const auto params       = GetParam();
  const auto impl         = std::get<0>(params);
  const auto binning_x    = std::get<1>(params);
  const auto binning_y    = std::get<2>(params);
  const auto test_pattern = std::get<3>(params);

  cv::Mat src = CreateTestData(test_pattern);
  cv::Mat ref = cv::Mat::zeros(cv::Size(src.cols / binning_x, src.rows / binning_y), src.type());
  cv::Mat dst = ref.clone();

  Binning<Impl::Naive> ref_impl;
  ref_impl.Execute(src, ref, binning_x, binning_y);
  impl->Execute(src, dst, binning_x, binning_y);

  for (auto y : std::views::iota(0, ref.rows)) {
    for (auto x : std::views::iota(0, ref.cols)) {
      ASSERT_EQ(ref.ptr<uint16_t>(y)[x], dst.ptr<uint16_t>(y)[x]) << std::format("(y, x)=({}, {})", y, x);
    }
  }
}
