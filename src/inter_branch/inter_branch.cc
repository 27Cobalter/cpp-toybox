#include "inter_branch.h"

#include <ranges>

#include <iostream>

void InterBranch::MultiOpInt(bool srli, bool slli, bool add, bool sub, bool bor, bool band,
                             int32_t size, const uint16_t* src, uint16_t& a, uint16_t& b,
                             uint16_t& c, uint16_t& d, uint16_t& e, uint16_t& f) {
  uint16_t aa = a;
  uint16_t bb = b;
  uint16_t cc = c;
  uint16_t dd = d;
  uint16_t ee = e;
  uint16_t ff = f;
  for (auto i : std::views::iota(0, size)) {
    if (srli) aa += (src[i] & 0b1);
    if (slli) bb += ((src[i] >> 1) & 0b1);
    if (add) cc +=  ((src[i] >> 2) & 0b1);
    if (sub) dd += ((src[i] >> 3) & 0b1);
    if (bor) ee += ((src[i] >> 4) & 0b1);
    if (band) ff += ((src[i] >> 5) & 0b1);
  }
  a = aa;
  b = bb;
  c = cc;
  d = dd;
  e = ee;
  f = ff;
}

template <bool srli, bool slli, bool add, bool sub, bool bor, bool band>
void MultiOpExt_Impl(int32_t size, const uint16_t* src, uint16_t& a, uint16_t& b,
                            uint16_t& c, uint16_t& d, uint16_t& e, uint16_t& f) {
  uint16_t aa = a;
  uint16_t bb = b;
  uint16_t cc = c;
  uint16_t dd = d;
  uint16_t ee = e;
  uint16_t ff = f;
  for (auto i : std::views::iota(0, size)) {
    uint16_t ss = src[i];
    if constexpr (srli) aa += (ss & 0b1);
    if constexpr (slli) bb += ((ss >> 1) & 0b1);
    if constexpr (add) cc +=  ((ss >> 2) & 0b1);
    if constexpr (sub) dd += ((ss >> 3) & 0b1);
    if constexpr (bor) ee += ((ss >> 4) & 0b1);
    if constexpr (band) ff += ((ss >> 5) & 0b1);
  }
  a = aa;
  b = bb;
  c = cc;
  d = dd;
  e = ee;
  f = ff;
}

template <bool... bools>
inline void MultiOpExt_Impl(bool head, auto&&... args) {
  if (head) {
    MultiOpExt_Impl<bools..., true>(std::forward<decltype(args)>(args)...);
  } else {
    MultiOpExt_Impl<bools..., false>(std::forward<decltype(args)>(args)...);
  }
}

void InterBranch::MultiOpExt(bool srli, bool slli, bool add, bool sub, bool bor, bool band,
                             int32_t size, const uint16_t* src, uint16_t& a, uint16_t& b,
                             uint16_t& c, uint16_t& d, uint16_t& e, uint16_t& f) {
  MultiOpExt_Impl<>(srli, slli, add, sub, bor, band, size, src, a, b, c, d, e, f);
}
