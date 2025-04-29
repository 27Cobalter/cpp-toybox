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

  // PrintValue<8, 8>(src);
  std::chrono::high_resolution_clock::time_point start, end;

  constexpr int32_t loop_time = 10;
#define MEASURE_BEGIN()                                           \
  std::fill(dst1x1.begin<uint16_t>(), dst1x1.end<uint16_t>(), 0); \
  std::fill(dst2x2.begin<uint16_t>(), dst2x2.end<uint16_t>(), 0); \
  std::fill(dst4x4.begin<uint16_t>(), dst4x4.end<uint16_t>(), 0); \
  start = std::chrono::high_resolution_clock::now();              \
  for (auto i : std::views::iota(0, loop_time)) {
#define MEASURE_END()                                                                   \
  }                                                                                     \
  end = std::chrono::high_resolution_clock::now();                                      \
  std::println(                                                                         \
      "{:6.3f}",                                                                        \
      static_cast<double>(                                                              \
          std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) / \
          loop_time / 1000);

  cv::Mat dst1x1 = cv::Mat(src.size() / 1, src.type());
  cv::Mat dst2x2 = cv::Mat(src.size() / 2, src.type());
  cv::Mat dst4x4 = cv::Mat(src.size() / 4, src.type());

  std::println("Navie");
  Binning<Impl::Naive> naive;
  MEASURE_BEGIN();
  naive.Execute(src, dst1x1, 1, 1);
  MEASURE_END();

  MEASURE_BEGIN();
  naive.Execute(src, dst2x2, 2, 2);
  MEASURE_END();

  MEASURE_BEGIN();
  naive.Execute(src, dst4x4, 4, 4);
  MEASURE_END();

  std::println("SeqRead");
  Binning<Impl::SeqRead> seqread;
  MEASURE_BEGIN();
  seqread.Execute(src, dst1x1, 1, 1);
  MEASURE_END();

  MEASURE_BEGIN();
  seqread.Execute(src, dst2x2, 2, 2);
  MEASURE_END();

  MEASURE_BEGIN();
  seqread.Execute(src, dst4x4, 4, 4);
  MEASURE_END();

  std::println("Avx512");
  Binning<Impl::Avx512> avx512;
  MEASURE_BEGIN();
  avx512.Execute(src, dst1x1, 1, 1);
  MEASURE_END();

  MEASURE_BEGIN();
  avx512.Execute(src, dst2x2, 2, 2);
  MEASURE_END();

  MEASURE_BEGIN();
  avx512.Execute(src, dst4x4, 4, 4);
  MEASURE_END();

  std::println("Avx512UnrollAll");
  Binning<Impl::Avx512UnrollAll> unrollall;
  MEASURE_BEGIN();
  unrollall.Execute(src, dst1x1, 1, 1);
  MEASURE_END();

  MEASURE_BEGIN();
  unrollall.Execute(src, dst2x2, 2, 2);
  MEASURE_END();

  MEASURE_BEGIN();
  unrollall.Execute(src, dst4x4, 4, 4);
  MEASURE_END();

  std::println("Avx512UnrollX");
  Binning<Impl::Avx512UnrollX> unrollx;
  MEASURE_BEGIN();
  unrollx.Execute(src, dst2x2, 2, 2);
  MEASURE_END();

  MEASURE_BEGIN();
  unrollx.Execute(src, dst4x4, 4, 4);
  MEASURE_END();

  std::println("Avx512UnrollLoad");
  Binning<Impl::Avx512UnrollLoad> unrollload;
  MEASURE_BEGIN();
  unrollload.Execute(src, dst2x2, 2, 2);
  MEASURE_END();

  MEASURE_BEGIN();
  unrollload.Execute(src, dst4x4, 4, 4);
  MEASURE_END();

  std::println("Avx512Seq");
  Binning<Impl::Avx512Seq> avx512seq;
  MEASURE_BEGIN();
  avx512seq.Execute(src, dst2x2, 2, 2);
  MEASURE_END();

  MEASURE_BEGIN();
  avx512seq.Execute(src, dst4x4, 4, 4);
  MEASURE_END();

  std::println("Avx512SeqBuffer");
  Binning<Impl::Avx512SeqBuffer> avx512seqbuffer;
  MEASURE_BEGIN();
  avx512seqbuffer.Execute(src, dst2x2, 2, 2);
  MEASURE_END();

  MEASURE_BEGIN();
  avx512seqbuffer.Execute(src, dst4x4, 4, 4);
  MEASURE_END();

  return 0;
}
