#include <algorithm>
#include <chrono>
#include <cstdint>
#include <memory>
#include <numeric>
#include <print>
#include <ranges>
#include <vector>

#include <opencv2/opencv.hpp>

auto main() -> int32_t {
  constexpr int32_t iteration = 1000;
  size_t size                 = 8192 * 8192;

  std::chrono::steady_clock::time_point start, end;
  std::vector<int32_t> duration[4];
  std::ranges::for_each(duration, [](auto& d) { d.reserve(iteration); });

  uint64_t sum0 = 0;
  uint64_t sum1 = 0;
  uint64_t sum2 = 0;
  uint64_t sum3 = 0;
  for (auto _ : std::views::iota(0, iteration)) {
    start            = std::chrono::high_resolution_clock::now();
    auto shared_init = std::make_shared<uint8_t[]>(size);
    end              = std::chrono::high_resolution_clock::now();
    duration[0].emplace_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());

    start            = std::chrono::high_resolution_clock::now();
    auto shared_over = std::make_shared_for_overwrite<uint8_t[]>(size);
    end              = std::chrono::high_resolution_clock::now();
    duration[1].emplace_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());

    start = std::chrono::high_resolution_clock::now();
    cv::Mat mat_init(8192, 8192, CV_8UC1);
    end = std::chrono::high_resolution_clock::now();
    duration[2].emplace_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
    start             = std::chrono::high_resolution_clock::now();
    cv::Mat mat_zeros = cv::Mat::zeros(8192, 8192, CV_8UC1);
    end               = std::chrono::high_resolution_clock::now();
    duration[3].emplace_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());

    sum0 += shared_init[0];
    sum0 += shared_init[size - 1];
    sum1 += shared_over[0];
    sum1 += shared_over[size - 1];
  }

  std::println("sum0: {}, sum1: {}", sum0, sum1);

  std::println("[0]: {}", std::reduce(duration[0].begin(), duration[0].end()));
  std::println("[1]: {}", std::reduce(duration[1].begin(), duration[1].end()));
  std::println("[2]: {}", std::reduce(duration[2].begin(), duration[2].end()));
  std::println("[3]: {}", std::reduce(duration[3].begin(), duration[3].end()));

  return 0;
}