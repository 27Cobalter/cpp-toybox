#include <chrono>
#include <print>
#include <ranges>
#include <vector>

#ifdef _MSC_VER
#define NOINLINE __declspec(noinline)
#else
#define NOINLINE
#endif

template<int32_t size>
struct Test {
  uint8_t data[size];
};

template<int32_t size>
NOINLINE int32_t CopyFunc(Test<size> test) {
  return test.data[0];
}

template<int32_t size>
NOINLINE int32_t RefFunc(Test<size>& test) {
  return test.data[0];
}

template<int32_t size>
NOINLINE int32_t PtrFunc(Test<size>* test) {
  return test->data[0];
}

template<int32_t size>
NOINLINE int32_t ConstCopyFunc(const Test<size> test) {
  return test.data[0];
}

template<int32_t size>
NOINLINE int32_t ConstRefFunc(const Test<size>& test) {
  return test.data[0];
}

template<int32_t size>
NOINLINE int32_t ConstPtrFunc(const Test<size>* test) {
  return test->data[0];
}

auto main() -> int {
  constexpr int32_t iter = 100000;
  constexpr int32_t loop = 1000;

  double time[24]     = {0};
  Test<1> size01      = {0};
  Test<16> size16     = {0};
  Test<32> size32     = {0};
  Test<2048> size2048 = {0};
  int32_t sum         = 0;

  std::chrono::high_resolution_clock::time_point start, end;

  for (auto it : std::views::iota(0, iter)) {
    int i = 0;

#define MEASURE_MACRO(x, y)                                                                \
  start = std::chrono::high_resolution_clock::now();                                       \
  for (auto lo : std::views::iota(0, loop)) {                                              \
    sum += x(y);                                                                           \
  }                                                                                        \
  end = std::chrono::high_resolution_clock::now();                                         \
  time[i++] += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    MEASURE_MACRO(CopyFunc, size01);
    MEASURE_MACRO(RefFunc, size01);
    MEASURE_MACRO(PtrFunc, &size01);
    MEASURE_MACRO(CopyFunc, size16);
    MEASURE_MACRO(RefFunc, size16);
    MEASURE_MACRO(PtrFunc, &size16);
    MEASURE_MACRO(CopyFunc, size32);
    MEASURE_MACRO(RefFunc, size32);
    MEASURE_MACRO(PtrFunc, &size32);
    MEASURE_MACRO(CopyFunc, size2048);
    MEASURE_MACRO(RefFunc, size2048);
    MEASURE_MACRO(PtrFunc, &size2048);
    MEASURE_MACRO(ConstCopyFunc, size01);
    MEASURE_MACRO(ConstRefFunc, size01);
    MEASURE_MACRO(ConstPtrFunc, &size01);
    MEASURE_MACRO(ConstCopyFunc, size16);
    MEASURE_MACRO(ConstRefFunc, size16);
    MEASURE_MACRO(ConstPtrFunc, &size16);
    MEASURE_MACRO(ConstCopyFunc, size32);
    MEASURE_MACRO(ConstRefFunc, size32);
    MEASURE_MACRO(ConstPtrFunc, &size32);
    MEASURE_MACRO(ConstCopyFunc, size2048);
    MEASURE_MACRO(ConstRefFunc, size2048);
    MEASURE_MACRO(ConstPtrFunc, &size2048);
  }
  int i = 0;
  std::println("CopyFunc    (size01) : {}", time[i++]);
  std::println("RefFunc     (size01) : {}", time[i++]);
  std::println("PtrFunc     (size01) : {}", time[i++]);
  std::println("CopyFunc    (size16)   : {}", time[i++]);
  std::println("RefFunc     (size16)   : {}", time[i++]);
  std::println("PtrFunc     (size16)   : {}", time[i++]);
  std::println("CopyFunc    (size32)   : {}", time[i++]);
  std::println("RefFunc     (size32)   : {}", time[i++]);
  std::println("PtrFunc     (size32)   : {}", time[i++]);
  std::println("CopyFunc    (size2048)   : {}", time[i++]);
  std::println("RefFunc     (size2048)   : {}", time[i++]);
  std::println("PtrFunc     (size2048)   : {}", time[i++]);
  std::println("CopyFunc C  (size01) : {}", time[i++]);
  std::println("RefFunc  C  (size01) : {}", time[i++]);
  std::println("PtrFunc  C  (size01) : {}", time[i++]);
  std::println("CopyFunc C  (size16)   : {}", time[i++]);
  std::println("RefFunc  C  (size16)   : {}", time[i++]);
  std::println("PtrFunc  C  (size16)   : {}", time[i++]);
  std::println("CopyFunc C  (size32)   : {}", time[i++]);
  std::println("RefFunc  C  (size32)   : {}", time[i++]);
  std::println("PtrFunc  C  (size32)   : {}", time[i++]);
  std::println("CopyFunc C  (size2048)   : {}", time[i++]);
  std::println("RefFunc  C  (size2048)   : {}", time[i++]);
  std::println("PtrFunc  C  (size2048)   : {}", time[i++]);
  std::println("sum : {}", sum);
  return 0;
}