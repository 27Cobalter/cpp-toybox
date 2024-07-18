#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <concepts>
#include <format>
#include <iostream>
#include <new>
#include <random>
#include <ranges>
#include <valarray>

#include <histo.h>

#include <opencv2/opencv.hpp>

template <typename T, typename U>
  requires std::convertible_to<T, double> && std::convertible_to<U, double>
double CalcMse(std::span<T> t, std::span<U> u) {
  assert(t.size() == u.size());
  double diff = 0;
  for (int i = 0; i < t.size(); i++) {
    diff += std::pow(t[i] - u[i], 2);

#ifndef NDEBUG
    std::cout << std::format("{:3}: {:6}, {:6}, {}", i, t[i], u[i], t[i] - u[i]) << std::endl;
#endif
  }
  return diff;
}

auto main() -> int {
  constexpr int32_t ALIGN_SIZE = 64;
  constexpr int32_t RANGE_MAX  = 0xFFFF;
  constexpr int32_t RANGE_SIZE = RANGE_MAX + 1;

  constexpr int32_t LOOP_COUNT = 1000;

  std::valarray<int32_t> resolution_list = {1024, 2048};
  // std::valarray<int32_t> resolution_list = {8192};

  decltype(std::chrono::high_resolution_clock::now()) start, end;
  std::random_device seed;
  std::mt19937 engine(seed());
  std::normal_distribution<> norm_dist(RANGE_SIZE >> 1, RANGE_SIZE / 10);

#ifdef _MSC_VER
  auto src_ptr = std::make_shared<uint16_t[]>(resolution_list.max() * resolution_list.max());
  auto ref_ptr = std::make_shared<int32_t[]>(RANGE_SIZE);
#else
  auto src_ptr = std::shared_ptr<uint16_t[]>(new (
      std::align_val_t(ALIGN_SIZE)) uint16_t[resolution_list.max() * resolution_list.max()]);
  auto ref_ptr =
      std::shared_ptr<int32_t[]>(new (std::align_val_t(ALIGN_SIZE)) int32_t[RANGE_SIZE]);
#endif
  std::span<int32_t> ref(ref_ptr.get(), RANGE_SIZE);

  for (auto resolution : resolution_list) {
    std::cout << std::format("data_size: {1}x{1}", resolution, resolution) << std::endl;
    const int32_t data_size = resolution * resolution;
    std::span<uint16_t> src(src_ptr.get(), data_size);

    cv::Mat mat_src(cv::Size(resolution, resolution), CV_16UC1);
    uint16_t* msptr = mat_src.ptr<uint16_t>(0, 0);

    for (auto& elem : src) {
      elem = static_cast<int32_t>(norm_dist(engine)) & RANGE_MAX;
    }
    for (int32_t i = 0; i < data_size; i++) {
      msptr[i] = src[i];
    }
    std::ranges::fill(ref, 0);
    for (auto& elem : src) {
      ref[elem]++;
    }

    MyHisto myhisto(RANGE_SIZE);
    start = std::chrono::high_resolution_clock::now();
    for (auto loop_i : std::views::iota(0, LOOP_COUNT)) {
      myhisto.Create_Impl<MyHisto::Method::Naive>(src.data(), src.size());
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << std::endl;
    std::cout << CalcMse(myhisto.histo_, ref) << std::endl;

    start = std::chrono::high_resolution_clock::now();
    for (auto loop_i : std::views::iota(0, LOOP_COUNT)) {
      myhisto.Create_Impl<MyHisto::Method::NaiveUnroll>(src.data(), src.size());
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << std::endl;
    std::cout << CalcMse(myhisto.histo_, ref) << std::endl;

    start = std::chrono::high_resolution_clock::now();
    for (auto loop_i : std::views::iota(0, LOOP_COUNT)) {
      myhisto.Create_Impl<MyHisto::Method::AVX512VPOPCNTDQ>(src.data(), src.size());
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << std::endl;
    std::cout << CalcMse(myhisto.histo_, ref) << std::endl;

    start = std::chrono::high_resolution_clock::now();
    for (auto loop_i : std::views::iota(0, LOOP_COUNT)) {
      myhisto.Create_Impl<MyHisto::Method::AVX512VPOPCNTDQ_Order>(src.data(), src.size());
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << std::endl;
    std::cout << CalcMse(myhisto.histo_, ref) << std::endl;

    start = std::chrono::high_resolution_clock::now();
    for (auto loop_i : std::views::iota(0, LOOP_COUNT)) {
      myhisto.Create_Impl<MyHisto::Method::Naive_MultiSubloop>(src.data(), src.size());
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << std::endl;
    std::cout << CalcMse(myhisto.histo_, ref) << std::endl;

    // for (int i = 0; i < RANGE_SIZE; i++) {
    //   std::cout << std::format("{:3}: {:6}, {:6}", i, ref[i], myhisto.histo_[i]) <<
    //   std::endl;
    // }

    // cv::MatND hist;
    // float hranges[] = {0, RANGE_SIZE};
    // const float* ranges[] = { hranges};
    // int histSize[] = {RANGE_SIZE};
    // start = std::chrono::high_resolution_clock::now();
    // for (auto loop_i : std::views::iota(0, LOOP_COUNT)) {
    //   cv::calcHist(&mat_src, 1, {0}, cv::Mat(), hist, 1, histSize, ranges, true, false);
    // }
    // end = std::chrono::high_resolution_clock::now();
    // std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
    //           << std::endl;
    // std::cout << CalcMse(myhisto.histo_, ref) << std::endl;
  }

  return 0;
}
