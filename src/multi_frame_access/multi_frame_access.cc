#include <multi_frame_access.h>
#include <ranges>

#include <immintrin.h>

template <typename T>
std::shared_ptr<T[]> make_aligned_array(int32_t size) {
#ifdef _MSC_VER
  return std::make_shared<T[]>(size);
#else
  return std::shared_ptr<T[]>(new (std::align_val_t(64)) T[size]);
#endif
}

constexpr int32_t step = 256 / 8 / sizeof(uint32_t);

SingleAlloc::SingleAlloc(int32_t width, int32_t height, int32_t frames) : current_(0), frames_(frames){
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

  for (int i = 0; i < data_size_; i += step) {
    __m256i srcV = _mm256_load_si256(reinterpret_cast<__m256i*>(src + i));
    __m256i accV = _mm256_load_si256(reinterpret_cast<__m256i*>(aptr + i));
    __m256i bufV = _mm256_load_si256(reinterpret_cast<__m256i*>(bptr + i));
    accV         = _mm256_add_epi32(accV, srcV);
    accV         = _mm256_sub_epi32(accV, bufV);
    _mm256_store_si256(reinterpret_cast<__m256i*>(aptr + i), accV);
    _mm256_store_si256(reinterpret_cast<__m256i*>(bptr + i), bufV);
  }
  return accumulate_.get();
}

MultiAlloc::MultiAlloc(int32_t width, int32_t height, int32_t frames) : current_(0),frames_(frames){
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

  for (int i = 0; i < data_size_; i += step) {
    __m256i srcV = _mm256_load_si256(reinterpret_cast<__m256i*>(src + i));
    __m256i accV = _mm256_load_si256(reinterpret_cast<__m256i*>(aptr + i));
    __m256i bufV = _mm256_load_si256(reinterpret_cast<__m256i*>(bptr + i));
    accV         = _mm256_add_epi32(accV, srcV);
    accV         = _mm256_sub_epi32(accV, bufV);
    _mm256_store_si256(reinterpret_cast<__m256i*>(aptr + i), accV);
    _mm256_store_si256(reinterpret_cast<__m256i*>(bptr + i), bufV);
  }

  return accumulate_.get();
}