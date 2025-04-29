#include <chrono>
#include <concepts>
#include <format>
#include <iostream>
#include <memory>
#include <new>
#include <numeric>
#include <random>
#include <ranges>
#include <span>
#include <stdfloat>
#include <valarray>

#include <immintrin.h>
#include <omp.h>

template<std::floating_point T, std::floating_point U>
std::float128_t mse(T* t, U* u, int32_t size) {
  std::float128_t diff = 0.0;
  for (auto i : std::views::iota(0, size)) {
    diff += (*t - *u) * (*t - *u);
  }
  return diff / size;
}

auto main() -> int {
  std::random_device seed;
  std::mt19937 generator(seed());

  constexpr int32_t data_size  = 2048;
  constexpr int32_t fast_count = 10000;
  constexpr int32_t slow_count = fast_count / 10;
  int32_t local_count          = fast_count;

  decltype(std::chrono::high_resolution_clock::now()) start, end;
  int32_t time = 0;

  std::shared_ptr<float[]> src(new (std::align_val_t(64)) float[data_size * data_size]);
  std::shared_ptr<float[]> dest(new (std::align_val_t(64)) float[data_size]);
  std::shared_ptr<std::float128_t[]> reference = std::make_shared<std::float128_t[]>(data_size);
  float* sptr                                  = src.get();
  float* dptr                                  = dest.get();
  std::float128_t* refptr                      = reference.get();

  for (auto i : std::views::iota(0, data_size * data_size)) {
    sptr[i] = generator() % 0x0FFF;
  }

  for (auto j : std::views::iota(0, data_size)) {
    refptr[j] = 0.0;
    for (auto i : std::views::iota(0, data_size)) {
      refptr[j] += sptr[data_size * j + i];
    }
  }

  std::cout << "single naive" << std::endl;
  local_count = slow_count;
  start       = std::chrono::high_resolution_clock::now();
  for (auto loop_i : std::views::iota(0, local_count)) {
    for (int j = 0; j < data_size; j++) {
      dptr[j] = 0.0f;
      for (int i = 0; i < data_size; i++) {
        dptr[j] += sptr[data_size * j + i];
      }
    }
  }
  end  = std::chrono::high_resolution_clock::now();
  time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / local_count;
  std::cout << std::format("{}: {}", time, mse(refptr, dptr, data_size)) << std::endl;

  std::cout << "multi naive" << std::endl;
  local_count = fast_count;
  start       = std::chrono::high_resolution_clock::now();
  for (auto loop_i : std::views::iota(0, local_count)) {
#pragma omp parallel for
    for (int j = 0; j < data_size; j++) {
      dptr[j] = 0.0f;
#pragma omp unroll
      for (int i = 0; i < data_size; i++) {
        dptr[j] += sptr[data_size * j + i];
      }
    }
  }
  end  = std::chrono::high_resolution_clock::now();
  time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / local_count;
  std::cout << std::format("{}: {}", time, mse(refptr, dptr, data_size)) << std::endl;

  std::cout << "single span+accumulate" << std::endl;
  local_count = slow_count;
  start       = std::chrono::high_resolution_clock::now();
  for (auto loop_i : std::views::iota(0, local_count)) {
    for (int j = 0; j < data_size; j++) {
      std::span<float> ssp(sptr + data_size * j, data_size);
      dptr[j] = std::accumulate(ssp.begin(), ssp.end(), 0.0f);
    }
  }
  end  = std::chrono::high_resolution_clock::now();
  time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / local_count;
  std::cout << std::format("{}: {}", time, mse(refptr, dptr, data_size)) << std::endl;

  std::cout << "multi span+accumulate" << std::endl;
  local_count = fast_count;
  start       = std::chrono::high_resolution_clock::now();
  for (auto loop_i : std::views::iota(0, local_count)) {
#pragma omp parallel for
    for (int j = 0; j < data_size; j++) {
      std::span<float> ssp(sptr + data_size * j, data_size);
      dptr[j] = std::accumulate(ssp.begin(), ssp.end(), 0.0f);
    }
  }
  end  = std::chrono::high_resolution_clock::now();
  time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / local_count;
  std::cout << std::format("{}: {}", time, mse(refptr, dptr, data_size)) << std::endl;

  std::cout << "single AVX2 load+hadd" << std::endl;
  local_count = fast_count;
  start       = std::chrono::high_resolution_clock::now();
  for (auto loop_i : std::views::iota(0, local_count)) {
    for (int j = 0; j < data_size; j++) {
      __m256 dv = _mm256_setzero_ps();
      for (int i = 0; i < data_size; i += 8) {
        dv = _mm256_add_ps(dv, _mm256_loadu_ps(sptr + data_size * j + i));
      }
      dv      = _mm256_hadd_ps(dv, dv);
      dv      = _mm256_hadd_ps(dv, dv);
      dptr[j] = reinterpret_cast<float*>(&dv)[0] + reinterpret_cast<float*>(&dv)[4];
    }
  }
  end  = std::chrono::high_resolution_clock::now();
  time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / local_count;
  std::cout << std::format("{}: {}", time, mse(refptr, dptr, data_size)) << std::endl;

  std::cout << "multi AVX2 load+hadd" << std::endl;
  local_count = fast_count;
  start       = std::chrono::high_resolution_clock::now();
  for (auto loop_i : std::views::iota(0, local_count)) {
#pragma omp parallel for
    for (int j = 0; j < data_size; j++) {
      __m256 dv = _mm256_setzero_ps();
#pragma omp unroll
      for (int i = 0; i < data_size; i += 8) {
        dv = _mm256_add_ps(dv, _mm256_loadu_ps(sptr + data_size * j + i));
      }
      dv      = _mm256_hadd_ps(dv, dv);
      dv      = _mm256_hadd_ps(dv, dv);
      dptr[j] = reinterpret_cast<float*>(&dv)[0] + reinterpret_cast<float*>(&dv)[4];
    }
  }
  end  = std::chrono::high_resolution_clock::now();
  time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / local_count;
  std::cout << std::format("{}: {}", time, mse(refptr, dptr, data_size)) << std::endl;

  std::cout << "single AVX-512 load+hadd" << std::endl;
  local_count = fast_count;
  start       = std::chrono::high_resolution_clock::now();
  for (auto loop_i : std::views::iota(0, local_count)) {
    for (int j = 0; j < data_size; j++) {
      __m512 dv = _mm512_setzero_ps();
      for (int i = 0; i < data_size; i += 16) {
        dv = _mm512_add_ps(dv, _mm512_loadu_ps(sptr + data_size * j + i));
      }
      __m256 low_v         = _mm512_castps512_ps256(dv);
      const __m256i high_v = _mm512_extracti64x4_epi64((__m512i)dv, 1);
      low_v                = _mm256_add_ps(low_v, (__m256)high_v);
      low_v                = _mm256_hadd_ps(low_v, low_v);
      low_v                = _mm256_hadd_ps(low_v, low_v);
      dptr[j]              = reinterpret_cast<float*>(&low_v)[0] + reinterpret_cast<float*>(&low_v)[4];
    }
  }
  end  = std::chrono::high_resolution_clock::now();
  time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / local_count;
  std::cout << std::format("{}: {}", time, mse(refptr, dptr, data_size)) << std::endl;

  std::cout << "multi AVX-512 load+hadd" << std::endl;
  local_count = fast_count;
  start       = std::chrono::high_resolution_clock::now();
  for (auto loop_i : std::views::iota(0, local_count)) {
#pragma omp parallel for
    for (int j = 0; j < data_size; j++) {
      __m512 dv = _mm512_setzero_ps();
#pragma omp unroll
      for (int i = 0; i < data_size; i += 16) {
        dv = _mm512_add_ps(dv, _mm512_loadu_ps(sptr + data_size * j + i));
      }
      __m256 low_v         = _mm512_castps512_ps256(dv);
      const __m256i high_v = _mm512_extracti64x4_epi64((__m512i)dv, 1);
      low_v                = _mm256_add_ps(low_v, (__m256)high_v);
      low_v                = _mm256_hadd_ps(low_v, low_v);
      low_v                = _mm256_hadd_ps(low_v, low_v);
      dptr[j]              = reinterpret_cast<float*>(&low_v)[0] + reinterpret_cast<float*>(&low_v)[4];
    }
  }
  end  = std::chrono::high_resolution_clock::now();
  time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / local_count;
  std::cout << std::format("{}: {}", time, mse(refptr, dptr, data_size)) << std::endl;

  std::cout << "multi AVX2 gather vadd" << std::endl;
  local_count = fast_count;
  start       = std::chrono::high_resolution_clock::now();
  for (auto loop_i : std::views::iota(0, local_count)) {
    const __m256i idxv = _mm256_setr_epi32(0, data_size * 1, data_size * 2, data_size * 3, data_size * 4, data_size * 5,
                                           data_size * 6, data_size * 7);
#pragma omp parallel for
    for (int j = 0; j < data_size; j += 8) {
      __m256 dv = _mm256_setzero_ps();
#pragma omp unroll
      for (int i = 0; i < data_size; i++) {
        dv = _mm256_add_ps(dv, _mm256_i32gather_ps(sptr + data_size * j + i, idxv, 4));
      }
      _mm256_storeu_ps(dptr + j, dv);
    }
  }
  end  = std::chrono::high_resolution_clock::now();
  time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / local_count;
  std::cout << std::format("{}: {}", time, mse(refptr, dptr, data_size)) << std::endl;

  return 0;
}
