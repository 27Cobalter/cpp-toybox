#include <chrono>
#include <valarray>
#include <format>
#include <iostream>
#include <memory>
#include <random>
#include <ranges>

#include <immintrin.h>
#include <omp.h>
#include <xmmintrin.h>

auto main() -> int {
  constexpr int fast_loop = 1000;
  constexpr int slow_loop = fast_loop / 1;
  int current_loop        = fast_loop;

  std::random_device seed;
  std::mt19937 generator(seed());

  int32_t data_size = 2048;

  std::shared_ptr<float[]> data(new (std::align_val_t(64)) float[data_size * data_size]);
  std::shared_ptr<float[]> result(new (std::align_val_t(64)) float[data_size]);
  float* sptr = data.get();
  float* dptr = result.get();
  std::printf("data %p, result %p\n", sptr, dptr);
  std::printf("data %lu, result %lu\n", reinterpret_cast<intptr_t>(sptr) % 512,
              reinterpret_cast<intptr_t>(dptr) % 64);

  for (auto i : std::views::iota(0, data_size * data_size)) {
    data[i] = generator();
  }

  std::cout << "single ji loop" << std::endl;
  current_loop = slow_loop;
  auto start   = std::chrono::high_resolution_clock::now();
  for (auto measure_i : std::views::iota(0, current_loop)) {
    for (int32_t i = 0; i < data_size; i++) {
      dptr[i] = 0.0;
    }
    for (int32_t j = 0; j < data_size; j++) {
      float* sptrj = sptr + data_size * j;
      for (int32_t i = 0; i < data_size; i++) {
        dptr[i] += sptrj[i];
      }
    }
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::cout << std::format(
                   "time: {} us",
                   std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                       current_loop)
            << std::endl;
  std::cout << std::format("sum: {}", std::valarray<float>(dptr, 2048).sum()) << std::endl;

  std::cout << "single ij loop" << std::endl;
  current_loop = slow_loop / 10;
  start        = std::chrono::high_resolution_clock::now();
  for (auto measure_i : std::views::iota(0, current_loop)) {
    for (int32_t i = 0; i < data_size; i++) {
      dptr[i] = 0.0f;
      for (int32_t j = 0; j < data_size; j++) {
        dptr[i] += sptr[data_size * j + i];
      }
    }
  }
  end = std::chrono::high_resolution_clock::now();
  std::cout << std::format(
                   "time: {} us",
                   std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                       current_loop)
            << std::endl;
  std::cout << std::format("sum: {}", std::valarray<float>(dptr, 2048).sum()) << std::endl;

  std::cout << "single iji loop" << std::endl;
  current_loop = slow_loop / 10;
  start        = std::chrono::high_resolution_clock::now();
  for (auto measure_i : std::views::iota(0, current_loop)) {
    for (int32_t i = 0; i < data_size; i += 16) {
      const int32_t ii_end = i + 16;
      for (int32_t ii = i; ii < ii_end; ii++) {
        dptr[ii] = 0.0;
      }
      for (int32_t j = 0; j < data_size; j++) {
        for (int32_t ii = i; ii < ii_end; ii++) {
          dptr[ii] += sptr[data_size * j + ii];
        }
      }
    }
  }
  end = std::chrono::high_resolution_clock::now();
  std::cout << std::format(
                   "time: {} us",
                   std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                       current_loop)
            << std::endl;
  std::cout << std::format("sum: {}", std::valarray<float>(dptr, 2048).sum()) << std::endl;

  std::cout << "omp ji loop" << std::endl;
  current_loop = fast_loop;
  start        = std::chrono::high_resolution_clock::now();
  for (auto measure_i : std::views::iota(0, current_loop)) {
#pragma omp parallel for
    for (int32_t i = 0; i < data_size; i += 16) {
#pragma omp unroll
      for (int32_t ii = i; ii < i + 16; ii++) {
        dptr[ii] = 0.0;
      }
    }
    for (int32_t j = 0; j < data_size; j++) {
      float* sptrj = sptr + data_size * j;
#pragma omp parallel for
      for (int32_t i = 0; i < data_size; i += 16) {
#pragma omp unroll
        for (int32_t ii = i; ii < i + 16; ii++) {
          dptr[ii] += sptrj[ii];
        }
      }
    }
  }
  end = std::chrono::high_resolution_clock::now();
  std::cout << std::format(
                   "time: {} us",
                   std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                       current_loop)
            << std::endl;
  std::cout << std::format("sum: {}", std::valarray<float>(dptr, 2048).sum()) << std::endl;

  std::cout << "omp ij loop" << std::endl;
  current_loop = fast_loop;
  start        = std::chrono::high_resolution_clock::now();
  for (auto measure_i : std::views::iota(0, current_loop)) {
#pragma omp parallel for
    for (int32_t i = 0; i < data_size; i++) {
      float* sptri = sptr + i;
      float* dptri = dptr + i;
      *dptri       = 0.0f;
#pragma omp unroll
      for (int32_t j = 0; j < data_size; j++) {
        *dptri += sptri[data_size * j];
      }
    }
  }
  end = std::chrono::high_resolution_clock::now();
  std::cout << std::format(
                   "time: {} us",
                   std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                       current_loop)
            << std::endl;
  std::cout << std::format("sum: {}", std::valarray<float>(dptr, 2048).sum()) << std::endl;

  std::cout << "omp iji loop" << std::endl;
  current_loop = fast_loop;
  start        = std::chrono::high_resolution_clock::now();
  for (auto measure_i : std::views::iota(0, current_loop)) {
#pragma omp parallel for
    for (int32_t i = 0; i < data_size; i += 16) {
      float* dptri = dptr + i;
#pragma omp unroll
      for (int32_t ii = 0; ii < 16; ii++) {
        dptri[ii] = 0.0f;
      }
#pragma omp unroll
      for (int32_t j = 0; j < data_size; j++) {
        float* sptrij = sptr + i + data_size * j;
#pragma omp unroll
        for (int ii = 0; ii < 16; ii++) {
          dptri[ii] += sptrij[ii];
        }
      }
    }
  }
  end = std::chrono::high_resolution_clock::now();
  std::cout << std::format(
                   "time: {} us",
                   std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                       current_loop)
            << std::endl;
  std::cout << std::format("sum: {}", std::valarray<float>(dptr, 2048).sum()) << std::endl;

  std::cout << "omp+AVX2 iji loop" << std::endl;
  current_loop = fast_loop;
  start        = std::chrono::high_resolution_clock::now();
  for (auto measure_i : std::views::iota(0, current_loop)) {
#pragma omp parallel for
    for (int32_t i = 0; i < data_size; i += 16) {
      __m256 dptri_v1 = _mm256_setzero_ps();
      __m256 dptri_v2 = _mm256_setzero_ps();
      float* dptri  =  dptr + i;
#pragma omp unroll
      for (int32_t j = 0; j < data_size; j++) {
        float* sptrij  =  sptr + i + data_size * j;
        dptri_v1      = _mm256_add_ps(dptri_v1, _mm256_loadu_ps(sptrij));
        dptri_v2      = _mm256_add_ps(dptri_v2, _mm256_loadu_ps(sptrij + 8));
      }
      _mm256_storeu_ps(dptri, dptri_v1);
      _mm256_storeu_ps(dptri + 8, dptri_v2);
    }
  }
  end = std::chrono::high_resolution_clock::now();
  std::cout << std::format(
                   "time: {} us",
                   std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                       current_loop)
            << std::endl;
  std::cout << std::format("sum: {}", std::valarray<float>(dptr, 2048).sum()) << std::endl;

  std::cout << "omp+AVX-512 iji loop" << std::endl;
  current_loop = fast_loop;
  start        = std::chrono::high_resolution_clock::now();
  for (auto measure_i : std::views::iota(0, current_loop)) {
#pragma omp parallel for
    for (int32_t i = 0; i < data_size; i += 16) {
      float* dptri   = dptr + i;
      float* sptri   = sptr + i;
      __m512 dptri_v = _mm512_setzero_ps();
#pragma omp unroll
      for (int32_t j = 0; j < data_size; j++) {
        float* sptrij = sptri + data_size * j;
        dptri_v       = _mm512_add_ps(dptri_v, _mm512_loadu_ps(sptrij));
      }
      _mm512_storeu_ps(dptri, dptri_v);
    }
  }
  end = std::chrono::high_resolution_clock::now();
  std::cout << std::format(
                   "time: {} us",
                   std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() /
                       current_loop)
            << std::endl;
  std::cout << std::format("sum: {}", std::valarray<float>(dptr, 2048).sum()) << std::endl;

  return 0;
}
