#include <chrono>
#include <cstdint>
#include <print>

constexpr int32_t rank = 50;

uint64_t fib_func(int32_t n) {
  if (n < 2) {
    return n;
  } else {
    return fib_func(n - 2) + fib_func(n - 1);
  }
}

template<int32_t N>
uint64_t fib_temp() {
  if constexpr (N < 2) {
    return N;
  } else {
    return fib_temp<N - 2>() + fib_temp<N - 1>();
  }
}

auto main() -> int32_t {
  {
    auto start = std::chrono::high_resolution_clock::now();
    auto ret   = fib_func(rank);
    auto end   = std::chrono::high_resolution_clock::now();
    auto count = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::println("{}: {}ms", ret, count);
  }

  {
    auto start = std::chrono::high_resolution_clock::now();
    auto ret   = fib_temp<rank>();
    auto end   = std::chrono::high_resolution_clock::now();
    auto count = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::println("{}: {}ms", ret, count);
  }
}