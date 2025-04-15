#include <ranges>
#include <print>
#include <chrono>

#include <omp.h>
#include <opencv2/opencv.hpp>

auto main() -> int32_t {
  constexpr int32_t kernel_size = 9;
  constexpr int32_t kernel_half = kernel_size >> 1;
  cv::Mat kernel                = cv::Mat::zeros(9, 9, CV_16UC1);
  for (auto i : std::views::iota(0, kernel.rows)) {
    for (auto j : std::views::iota(0, kernel.cols)) {
      kernel.at<uint16_t>(i, j) = i + j;
    }
  }

  constexpr int32_t loop = 1;

  cv::setNumThreads(8);
  cv::Mat img = cv::Mat::zeros(2304, 2304, CV_16UC1);

  auto start = std::chrono::high_resolution_clock::now();
  for (auto t : std::views::iota(0, loop)) {
	  cv::filter2D(img, img, CV_16UC1, kernel, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::println("Time taken: {} ms",
               std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

  start = std::chrono::high_resolution_clock::now();
  for (auto t : std::views::iota(0, loop)) {
    cv::Mat pad;
    double kernel_sum = 0.0;
    double kernel_rcp = 1.0 / kernel_sum;
    for (auto i : std::views::iota(0, kernel.rows)) {
      for (auto j : std::views::iota(0, kernel.cols)) {
        kernel_sum += kernel.at<uint16_t>(i, j);
      }
    }
    cv::copyMakeBorder(img, pad, kernel_half, kernel_half, kernel_half, kernel_half,
                       cv::BORDER_REFLECT);

#pragma omp parallel for
    for (int32_t i = 0; i < img.rows; i++) {
      auto* dptr = img.ptr<uint16_t>(i);
      for (auto j : std::views::iota(0, img.cols)) {
        float sum = 0;
        for (auto k : std::views::iota(0, kernel_size)) {
          auto* sptr = pad.ptr<uint16_t>(i + k);
          auto* kptr = kernel.ptr<uint16_t>(k);
          for (auto l : std::views::iota(0, kernel_size)) {
            sum += sptr[j + l] * kptr[l];
          }
        }
        img.at<uint16_t>(i, j) = sum * kernel_rcp;
      }
    }
  }
  end = std::chrono::high_resolution_clock::now();
  std::println("Time taken: {} ms",
               std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

  return 0;
}
