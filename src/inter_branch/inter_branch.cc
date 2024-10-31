#include "inter_branch.h"

#include <ranges>

void InterBranch::MultiOpInt(bool srli, bool slli, bool add, bool sub, bool bor, bool band,
                             int32_t size, const uint16_t* src, uint16_t* a, uint16_t* b,
                             uint16_t* c, uint16_t* d, uint16_t* e, uint16_t* f) {
  for (auto i : std::views::iota(0, size)) {
    if (srli) a[i] = src[i] >> 1;
    if (slli) b[i] = src[i] << 1;
    if (add) c[i] = src[i] + 1;
    if (sub) d[i] = src[i] - 1;
    if (bor) e[i] = src[i] | 1;
    if (band) f[i] = src[i] & 1;
  }
}

template <bool srli, bool slli, bool add, bool sub, bool bor, bool band>
void MultiOpExt_Impl(int32_t size, const uint16_t* src, uint16_t* a, uint16_t* b,
                            uint16_t* c, uint16_t* d, uint16_t* e, uint16_t* f) {
  for (auto i : std::views::iota(0, size)) {
    if constexpr (srli) a[i] = src[i] >> 1;
    if constexpr (slli) b[i] = src[i] << 1;
    if constexpr (add) c[i] = src[i] + 1;
    if constexpr (sub) d[i] = src[i] - 1;
    if constexpr (bor) e[i] = src[i] | 1;
    if constexpr (band) f[i] = src[i] & 1;
  }
}

template <bool... bools>
inline void MultiOpExt_Impl(bool head, auto... args) {
  if (head) {
    MultiOpExt_Impl<bools..., true>(args...);
  } else {
    MultiOpExt_Impl<bools..., false>(args...);
  }
}

void InterBranch::MultiOpExt(bool srli, bool slli, bool add, bool sub, bool bor, bool band,
                             int32_t size, const uint16_t* src, uint16_t* a, uint16_t* b,
                             uint16_t* c, uint16_t* d, uint16_t* e, uint16_t* f) {
  MultiOpExt_Impl<>(srli, slli, add, sub, bor, band, size, src, a, b, c, d, e, f);
}
