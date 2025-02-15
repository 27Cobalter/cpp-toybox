#include <chrono>
#include <cstdint>
#include <print>
#include <ranges>

#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>

#include "binning.h"

template <int32_t X, int32_t Y>
void PrintValue(const cv::Mat& src) {
  std::println("{}x{}", X, Y);
  for (auto y : std::views::iota(0, Y)) {
    for (auto x : std::views::iota(0, X)) {
      std::print("{:5d}, ", src.ptr<uint16_t>(y)[x]);
    }
    std::println("");
  }
}

auto main() -> int {
  cv::Mat src = cv::Mat(cv::Size(4096, 4096), CV_16UC1);
  for (auto y : std::views::iota(0, src.rows)) {
    uint16_t* sptry = src.ptr<uint16_t>(y);
    for (auto x : std::views::iota(0, src.cols)) {
      sptry[x] = y + x;
    }
  }

  PrintValue<8, 8>(src);
  std::chrono::high_resolution_clock::time_point start, end;

#define MEASURE_BEGIN()                                           \
  std::println("");                                               \
  std::fill(dst1x1.begin<uint16_t>(), dst1x1.end<uint16_t>(), 0); \
  std::fill(dst2x2.begin<uint16_t>(), dst2x2.end<uint16_t>(), 0); \
  std::fill(dst4x4.begin<uint16_t>(), dst4x4.end<uint16_t>(), 0); \
  start = std::chrono::high_resolution_clock::now();
#define MEASURE_END()                                                                         \
  end = std::chrono::high_resolution_clock::now();                                            \
  std::println(                                                                               \
      "{}", static_cast<double>(                                                              \
                std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) / \
                1000);

  cv::Mat dst1x1 = cv::Mat(src.size() / 1, src.type());
  cv::Mat dst2x2 = cv::Mat(src.size() / 2, src.type());
  cv::Mat dst4x4 = cv::Mat(src.size() / 4, src.type());

  MEASURE_BEGIN();
  Binning<Impl::Naive>::Execute<1, 1>(src, dst1x1);
  MEASURE_END();
  PrintValue<20, 2>(dst1x1);

  MEASURE_BEGIN();
  Binning<Impl::Naive>::Execute<2, 2>(src, dst2x2);
  MEASURE_END();
  PrintValue<20, 2>(dst2x2);

  MEASURE_BEGIN();
  Binning<Impl::Naive>::Execute<4, 4>(src, dst4x4);
  MEASURE_END();
  PrintValue<20, 2>(dst4x4);

  MEASURE_BEGIN();
  Binning<Impl::SeqRead>::Execute<1, 1>(src, dst1x1);
  MEASURE_END();
  PrintValue<20, 2>(dst1x1);

  MEASURE_BEGIN();
  Binning<Impl::SeqRead>::Execute<2, 2>(src, dst2x2);
  MEASURE_END();
  PrintValue<20, 2>(dst2x2);

  MEASURE_BEGIN();
  Binning<Impl::SeqRead>::Execute<4, 4>(src, dst4x4);
  MEASURE_END();
  PrintValue<20, 2>(dst4x4);

  MEASURE_BEGIN();
  Binning<Impl::Avx512>::Execute<1, 1>(src, dst1x1);
  MEASURE_END();
  PrintValue<20, 2>(dst1x1);

  MEASURE_BEGIN();
  Binning<Impl::Avx512>::Execute<2, 2>(src, dst2x2);
  MEASURE_END();
  PrintValue<20, 2>(dst2x2);

  MEASURE_BEGIN();
  Binning<Impl::Avx512>::Execute<4, 4>(src, dst4x4);
  MEASURE_END();
  PrintValue<20, 2>(dst4x4);

  MEASURE_BEGIN();
  Binning<Impl::Avx512UnrollAll>::Execute<1, 1>(src, dst1x1);
  MEASURE_END();
  PrintValue<20, 2>(dst1x1);

  MEASURE_BEGIN();
  Binning<Impl::Avx512UnrollAll>::Execute<2, 2>(src, dst2x2);
  MEASURE_END();
  PrintValue<20, 2>(dst2x2);

  MEASURE_BEGIN();
  Binning<Impl::Avx512UnrollAll>::Execute<4, 4>(src, dst4x4);
  MEASURE_END();
  PrintValue<20, 2>(dst4x4);

  MEASURE_BEGIN();
  Binning<Impl::Avx512UnrollX>::Execute<2, 2>(src, dst2x2);
  MEASURE_END();
  PrintValue<20, 2>(dst2x2);

  MEASURE_BEGIN();
  Binning<Impl::Avx512UnrollX>::Execute<4, 4>(src, dst4x4);
  MEASURE_END();
  PrintValue<20, 2>(dst4x4);

  MEASURE_BEGIN();
  Binning<Impl::Avx512Seq>::Execute<2, 2>(src, dst2x2);
  MEASURE_END();
  PrintValue<20, 2>(dst2x2);

  MEASURE_BEGIN();
  Binning<Impl::Avx512Seq>::Execute<4, 4>(src, dst4x4);
  MEASURE_END();
  PrintValue<20, 2>(dst4x4);

  MEASURE_BEGIN();
  Binning<Impl::Avx512SeqBuffer>::Execute<2, 2>(src, dst2x2);
  MEASURE_END();
  PrintValue<20, 2>(dst2x2);

  MEASURE_BEGIN();
  Binning<Impl::Avx512SeqBuffer>::Execute<4, 4>(src, dst4x4);
  MEASURE_END();
  PrintValue<20, 2>(dst4x4);

  return 0;
}
