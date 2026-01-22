#include <algorithm>
#include <chrono>
#include <cstdint>
#include <numeric>
#include <print>
#include <ranges>
#include <vector>

#include <opencv2/opencv.hpp>

auto main() -> int32_t {
  int32_t width  = 4096;
  int32_t height = 4096;
  cv::Mat src(cv::Size(width, height), CV_8UC1);
  for (auto y : std::views::iota(0, height)) {
    auto ptr = src.ptr<uint8_t>(y);
    for (auto x : std::views::iota(0, width)) {
      ptr[x] = static_cast<uint8_t>(x + y);
    }
  }

  int32_t iterations = 100;
  int32_t pad        = 5;

  std::chrono::steady_clock::time_point start, end;
  std::vector<int32_t> border_type = {cv::BORDER_CONSTANT, cv::BORDER_REPLICATE, cv::BORDER_REFLECT, cv::BORDER_WRAP,
                                      cv::BORDER_REFLECT_101};
  std::vector<std::string> border_type_name = {"BORDER_CONSTANT", "BORDER_REPLICATE", "BORDER_REFLECT", "BORDER_WRAP",
                                               "BORDER_REFLECT_101"};
  std::vector<std::vector<int64_t>> durations(border_type.size());
  durations.resize(border_type.size());
  std::ranges::for_each(durations, [&](auto e) { e.reserve(iterations); });

  std::vector<cv::Mat> dsts(border_type.size());

  for (auto _ : std::views::iota(0, iterations)) {
    for (auto [type, dst, dur] : std::views::zip(border_type, dsts, durations)) {
      start = std::chrono::steady_clock::now();
      cv::copyMakeBorder(src, dst, pad, pad, pad, pad, type, 120);
      end = std::chrono::steady_clock::now();
      dur.emplace_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
    }
  }

  for (auto [type_name, dst, dur] : std::views::zip(border_type_name, dsts, durations)) {
    auto total = std::reduce(dur.begin(), dur.end());
    std::print("{}: {} us\n", type_name, total / iterations);

    cv::imshow(type_name, dst);
  }
  cv::waitKey(0);

  return 0;
}