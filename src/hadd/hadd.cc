// #include <omp.h>

#include <chrono>
#include <iostream>
#include <numeric>
#include <ranges>
#include <vector>

#define measure_start()                                     \
  {                                                         \
    auto start = std::chrono::high_resolution_clock::now(); \
    for (auto measure_i = 0; measure_i < measure_count; measure_i++) {
#define measure_end()                                                                     \
  }                                                                                       \
  auto end = std::chrono::high_resolution_clock::now();                                   \
  std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() \
            << std::endl;                                                                 \
  }

constexpr int32_t measure_count = 1000;

auto main() -> int {
  std::vector<int32_t> vec(measure_count, 2);

  int32_t sum = 0;
  std::cout << "range for" << std::endl;
  measure_start();
  sum = 0;
  for (auto elem : vec) {
    sum += elem;
  }
  measure_end();
  std::cout << sum << std::endl;

  std::cout << "index for" << std::endl;
  measure_start();
  sum = 0;
  for (int32_t i = 0; i < vec.size(); i++) {
    sum += vec[i];
  }
  measure_end();
  std::cout << sum << std::endl;

  std::cout << "iota for" << std::endl;
  measure_start();
  sum = 0;
  for (auto i : std::views::iota(0, measure_count)) {
    sum += vec[i];
  }
  measure_end();
  std::cout << sum << std::endl;

  // sum = 0;
  // std::cout << "index for omp" << std::endl;
  // measure_start();
  // #pragma omp parallel for
  // for(int32_t i = 0; i < vec.size(); i++){
  //   sum += vec[i];
  // }
  // measure_end();
  // std::cout << sum << std::endl;

  std::cout << "accumulate" << std::endl;
  measure_start();
  sum = std::accumulate(vec.begin(), vec.end(), 0);
  measure_end();
  std::cout << sum << std::endl;

  std::cout << "reduce" << std::endl;
  measure_start();
  sum = std::reduce(vec.begin(), vec.end(), 0);
  measure_end();
  std::cout << sum << std::endl;

  return 0;
}
