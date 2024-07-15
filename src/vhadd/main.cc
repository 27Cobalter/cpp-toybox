#include <cassert>
#include <chrono>
#include <concepts>
#include <format>
#include <iostream>
#include <limits>
#include <new>
#include <numeric>
#include <print>
#include <random>
#include <ranges>
#include <span>
#include <valarray>

#include <opencv2/core/utility.hpp>
#include <opencv2/opencv.hpp>

#include <InstructionInfo.h>

#include "vhadd.h"

template <typename T, typename U>
requires std::convertible_to<T, float> && std::convertible_to<U, float>
float CalcDiff(T* t, U* u, int32_t data_size) {
  float diff = 0;
  for (auto i : std::views::iota(0, data_size)) {
    diff += (t[i] - u[i]) * (t[i] - u[i]);
    if (std::abs(static_cast<float>(t[i] - u[i])) > 0.1) {
      // std::cout << std::format("(t[{}], u[{}]) = ({}, {})", i, i, t[i], u[i]) << std::endl;
    }
  }
  return diff / data_size;
}

auto main() -> int {
  cv::setNumThreads(1);
#ifdef _MSC_VER
  constexpr int32_t loop_count = 10000;
  std::valarray<int32_t> width_samples{1024};
#else
  constexpr int32_t loop_count = 100000;
  std::valarray<int32_t> width_samples{1024};
#endif

  using IIIS                      = InstructionInfo::InstructionSet;
  const bool supported_avx2       = InstructionInfo::IsSupported(IIIS::AVX2);
  const bool supported_avx512f    = InstructionInfo::IsSupported(IIIS::AVX512F);
  const bool supported_avx512vbmi = InstructionInfo::IsSupported(IIIS::AVX512_VBMI);

  std::chrono::high_resolution_clock::time_point start, end;
  int32_t time_count;
  float diff;

  constexpr int32_t RANGE_MAX  = std::numeric_limits<uint16_t>::max();
  constexpr int32_t RANGE_SIZE = RANGE_MAX + 1;

  std::random_device seed;
  std::mt19937 gen(seed());
  std::normal_distribution<> norm(RANGE_SIZE >> 2, RANGE_SIZE / 9);

  for (auto width : width_samples) {
    cv::Mat src   = cv::Mat::zeros(cv::Size(width, width), CV_16UC1);
    cv::Mat h_ref;
    cv::Mat v_ref;

    std::span<uint16_t> h_dst;
    std::span<uint16_t> v_dst;

    const int32_t data_size = width * width;

    uint16_t* sptr  = src.ptr<uint16_t>(0);
    for (auto j : std::views::iota(0, src.rows)) {
      for (auto i : std::views::iota(0, src.cols)) {
        sptr[j * width + i] = RANGE_MAX * static_cast<float>(i) / (width - 1);
      }
    }

    VHAdd vhadd(src.cols, src.rows);

    std::string title;
    title = "reducce_v";
    start = std::chrono::high_resolution_clock::now();
    for (auto i : std::views::iota(0, loop_count)) {
      cv::reduce(src, v_ref, 0, cv::REDUCE_AVG, CV_32F);
    }
    end = std::chrono::high_resolution_clock::now();
    std::println("{}: {} us", title,
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
        loop_count);

    title = "reducce_h";
    start = std::chrono::high_resolution_clock::now();
    for (auto i : std::views::iota(0, loop_count)) {
      cv::reduce(src, h_ref, 1, cv::REDUCE_AVG, CV_32F);
    }
    end = std::chrono::high_resolution_clock::now();
    std::println("{}: {} us", title,
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
        loop_count);

    title = "reducce_vh";
    start = std::chrono::high_resolution_clock::now();
    for (auto i : std::views::iota(0, loop_count)) {
      cv::reduce(src, v_ref, 0, cv::REDUCE_AVG, CV_32F);
      cv::reduce(src, h_ref, 1, cv::REDUCE_AVG, CV_32F);
    }
    end = std::chrono::high_resolution_clock::now();
    std::println("{}: {} us", title,
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
        loop_count);

    title = "CalcV Naive";
    start = std::chrono::high_resolution_clock::now();
    for (auto i : std::views::iota(0, loop_count)) {
      v_dst = vhadd.CalcV_Impl<VHAdd::Method::Naive>(src.ptr<uint16_t>(0), src.cols * src.rows,
          0, 0, src.cols, src.rows);
    }
    end = std::chrono::high_resolution_clock::now();
    std::println("{}: {} us", title,
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
        loop_count);

    title = "CalcH Naive";
    start = std::chrono::high_resolution_clock::now();
    for (auto i : std::views::iota(0, loop_count)) {
      h_dst = vhadd.CalcH_Impl<VHAdd::Method::Naive>(src.ptr<uint16_t>(0), src.cols * src.rows,
          0, 0, src.cols, src.rows);
    }
    end = std::chrono::high_resolution_clock::now();
    std::println("{}: {} us", title,
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
        loop_count);

    title = "CalcV AVX2";
    start = std::chrono::high_resolution_clock::now();
    for (auto i : std::views::iota(0, loop_count)) {
      v_dst = vhadd.CalcV_Impl<VHAdd::Method::AVX2>(src.ptr<uint16_t>(0), src.cols * src.rows,
          0, 0, src.cols, src.rows);
    }
    end = std::chrono::high_resolution_clock::now();
    std::println("{}: {} us", title,
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
        loop_count);

    title = "CalcH AVX2";
    start = std::chrono::high_resolution_clock::now();
    for (auto i : std::views::iota(0, loop_count)) {
      h_dst = vhadd.CalcH_Impl<VHAdd::Method::AVX2>(src.ptr<uint16_t>(0), src.cols * src.rows,
          0, 0, src.cols, src.rows);
    }
    end = std::chrono::high_resolution_clock::now();
    std::println("{}: {} us", title,
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
        loop_count);

    title = "CalcVH AVX2";
    start = std::chrono::high_resolution_clock::now();
    for (auto i : std::views::iota(0, loop_count)) {
      auto [v_slice, h_slice] = vhadd.CalcVH_Impl<VHAdd::Method::AVX2>(src.ptr<uint16_t>(0), src.cols * src.rows,
          0, 0, src.cols, src.rows);

      h_dst = h_slice;
      v_dst = v_slice;
    }
    end = std::chrono::high_resolution_clock::now();
    std::println("{}: {} us", title,
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
        loop_count);

    title = "CalcV AVX2_V";
    start = std::chrono::high_resolution_clock::now();
    for (auto i : std::views::iota(0, loop_count)) {
      v_dst = vhadd.CalcV_Impl<VHAdd::Method::AVX2_Vertical>(src.ptr<uint16_t>(0), src.cols * src.rows,
          0, 0, src.cols, src.rows);
    }
    end = std::chrono::high_resolution_clock::now();
    std::println("{}: {} us", title,
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
        loop_count);

    title = "CalcV AVX512";
    start = std::chrono::high_resolution_clock::now();
    for (auto i : std::views::iota(0, loop_count)) {
      v_dst = vhadd.CalcV_Impl<VHAdd::Method::AVX512>(src.ptr<uint16_t>(0), src.cols * src.rows,
          0, 0, src.cols, src.rows);
    }
    end = std::chrono::high_resolution_clock::now();
    std::println("{}: {} us", title,
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
        loop_count);

    title = "CalcH AVX512";
    start = std::chrono::high_resolution_clock::now();
    for (auto i : std::views::iota(0, loop_count)) {
      h_dst = vhadd.CalcH_Impl<VHAdd::Method::AVX512>(src.ptr<uint16_t>(0), src.cols * src.rows,
          0, 0, src.cols, src.rows);
    }
    end = std::chrono::high_resolution_clock::now();
    std::println("{}: {} us", title,
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
        loop_count);

    title = "CalcVH AVX512";
    start = std::chrono::high_resolution_clock::now();
    for (auto i : std::views::iota(0, loop_count)) {
      auto [v_slice, h_slice] = vhadd.CalcVH_Impl<VHAdd::Method::AVX512>(src.ptr<uint16_t>(0), src.cols * src.rows,
          0, 0, src.cols, src.rows);

      h_dst = h_slice;
      v_dst = v_slice;
    }
    end = std::chrono::high_resolution_clock::now();
    std::println("{}: {} us", title,
        std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
        loop_count);

    // cv::imshow("src", src);
    // for (auto i : std::views::iota(0, h_ref.size().area())) {
    //   std::println("[i]: {}, v: {}, h: {}, vd: {}, hd: {}, diff: ({}, {})", i,
    //       v_ref.ptr<float>(0)[i], h_ref.ptr<float>(0)[i], v_dst[i], h_dst[i],
    //       static_cast<int32_t>(v_ref.ptr<float>(0)[i] - v_dst[i]), static_cast<int32_t>(h_ref.ptr<float>(0)[i] - h_dst[i])
    //       );
    // }
    // cv::waitKey();
  }

  return 0;
}
