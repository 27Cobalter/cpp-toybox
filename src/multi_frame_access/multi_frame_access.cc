#include <multi_frame_access.h>
#include <ranges>

#include <immintrin.h>

template<typename T>
std::shared_ptr<T[]> make_aligned_array(int32_t size) {
#ifdef _MSC_VER
  return std::make_shared<T[]>(size);
#else
  return std::shared_ptr<T[]>(new (std::align_val_t(64)) T[size]);
#endif
}

constexpr int32_t step = 256 / 8 / sizeof(uint32_t);

SingleAlloc::SingleAlloc(int32_t width, int32_t height, int32_t frames) : current_(0), frames_(frames) {
  round_buffer_.resize(frames);
  data_size_                = width * height;
  int32_t data_size         = width * height * sizeof(uint32_t);
  int32_t aligned_data_size = ((data_size + 63) / 64) * 64 / sizeof(uint32_t);
  round_ptr_                = make_aligned_array<uint32_t>(aligned_data_size * frames);
  uint32_t* rptr            = round_ptr_.get();

  for (auto i : std::views::iota(0, frames)) {
    round_buffer_[i] = std::span<uint32_t>(rptr + i * aligned_data_size, data_size);
  }
  accumulate_ = make_aligned_array<uint32_t>(data_size_);
}
uint32_t* SingleAlloc::Add(uint32_t* src) {
  current_ = (current_ + 1) % frames_;

  uint32_t* bptr = round_buffer_[current_].data();
  uint32_t* aptr = accumulate_.get();

  uint32_t pref_step = step * 10;
  for (int i = 0; i < data_size_; i += step) {
    __m256i srcV = _mm256_loadu_si256(reinterpret_cast<__m256i*>(src + i));
    __m256i accV = _mm256_load_si256(reinterpret_cast<__m256i*>(aptr + i));
    _mm_prefetch(reinterpret_cast<const char*>(aptr + i + pref_step), _MM_HINT_T0);
    __m256i bufV = _mm256_load_si256(reinterpret_cast<__m256i*>(bptr + i));
    _mm_prefetch(reinterpret_cast<const char*>(bptr + i + pref_step), _MM_HINT_T0);
    accV = _mm256_add_epi32(accV, srcV);
    accV = _mm256_sub_epi32(accV, bufV);
    _mm256_store_si256(reinterpret_cast<__m256i*>(aptr + i), accV);
    _mm256_store_si256(reinterpret_cast<__m256i*>(bptr + i), bufV);
  }
  return accumulate_.get();
}

MultiAlloc::MultiAlloc(int32_t width, int32_t height, int32_t frames) : current_(0), frames_(frames) {
  round_buffer_.resize(frames);
  data_size_ = width * height;

  for (auto i : std::views::iota(0, frames)) {
    round_buffer_[i] = make_aligned_array<uint32_t>(data_size_);
  }
  accumulate_ = make_aligned_array<uint32_t>(data_size_);
}

uint32_t* MultiAlloc::Add(uint32_t* src) {
  current_ = (current_ + 1) % frames_;

  uint32_t* bptr = round_buffer_[current_].get();
  uint32_t* aptr = accumulate_.get();

  int step2 = step * 2;
  int step3 = step * 3;
  int step4 = step * 4;

  uint32_t pref_step = step * 10;
  __m256i srcV0      = _mm256_loadu_si256(reinterpret_cast<__m256i*>(src));
  __m256i accV0      = _mm256_load_si256(reinterpret_cast<__m256i*>(aptr));
  __m256i bufV0      = _mm256_load_si256(reinterpret_cast<__m256i*>(bptr));
  __m256i srcV1      = _mm256_loadu_si256(reinterpret_cast<__m256i*>(src + step));
  __m256i accV1      = _mm256_load_si256(reinterpret_cast<__m256i*>(aptr + step));
  __m256i bufV1      = _mm256_load_si256(reinterpret_cast<__m256i*>(bptr + step));

  __m256i srcV2 = _mm256_loadu_si256(reinterpret_cast<__m256i*>(src + step2));
  __m256i accV2 = _mm256_load_si256(reinterpret_cast<__m256i*>(aptr + step2));
  __m256i bufV2 = _mm256_load_si256(reinterpret_cast<__m256i*>(bptr + step2));
  __m256i srcV3 = _mm256_loadu_si256(reinterpret_cast<__m256i*>(src + step3));
  __m256i accV3 = _mm256_load_si256(reinterpret_cast<__m256i*>(aptr + step3));
  __m256i bufV3 = _mm256_load_si256(reinterpret_cast<__m256i*>(bptr + step3));
  for (int i = 0; i < data_size_; i += step4) {
    accV0 = _mm256_add_epi32(accV0, srcV0);
    accV0 = _mm256_sub_epi32(accV0, bufV0);

    _mm256_store_si256(reinterpret_cast<__m256i*>(bptr + i), bufV0);
    _mm256_store_si256(reinterpret_cast<__m256i*>(aptr + i), accV0);
    srcV0 = _mm256_loadu_si256(reinterpret_cast<__m256i*>(src + i + step4));
    accV0 = _mm256_load_si256(reinterpret_cast<__m256i*>(aptr + i + step4));
    bufV0 = _mm256_load_si256(reinterpret_cast<__m256i*>(bptr + i + step4));

    accV1 = _mm256_add_epi32(accV1, srcV1);
    accV1 = _mm256_sub_epi32(accV1, bufV1);

    _mm256_store_si256(reinterpret_cast<__m256i*>(bptr + i + step), bufV1);
    _mm256_store_si256(reinterpret_cast<__m256i*>(aptr + i + step), accV1);
    srcV1 = _mm256_loadu_si256(reinterpret_cast<__m256i*>(src + i + step + step4));
    accV1 = _mm256_load_si256(reinterpret_cast<__m256i*>(aptr + i + step + step4));
    bufV1 = _mm256_load_si256(reinterpret_cast<__m256i*>(bptr + i + step + step4));

    accV2 = _mm256_add_epi32(accV2, srcV2);
    accV2 = _mm256_sub_epi32(accV2, bufV2);

    _mm256_store_si256(reinterpret_cast<__m256i*>(bptr + i + step2), bufV2);
    _mm256_store_si256(reinterpret_cast<__m256i*>(aptr + i + step2), accV2);
    srcV2 = _mm256_loadu_si256(reinterpret_cast<__m256i*>(src + i + step2 + step4));
    accV2 = _mm256_load_si256(reinterpret_cast<__m256i*>(aptr + i + step2 + step4));
    bufV2 = _mm256_load_si256(reinterpret_cast<__m256i*>(bptr + i + step2 + step4));
    accV2 = _mm256_add_epi32(accV1, srcV1);
    accV2 = _mm256_sub_epi32(accV1, bufV1);

    _mm256_store_si256(reinterpret_cast<__m256i*>(bptr + i + step3), bufV3);
    _mm256_store_si256(reinterpret_cast<__m256i*>(aptr + i + step3), accV3);
    srcV3 = _mm256_loadu_si256(reinterpret_cast<__m256i*>(src + i + step3 + step4));
    accV3 = _mm256_load_si256(reinterpret_cast<__m256i*>(aptr + i + step3 + step4));
    bufV3 = _mm256_load_si256(reinterpret_cast<__m256i*>(bptr + i + step3 + step4));
  }

  return accumulate_.get();
}