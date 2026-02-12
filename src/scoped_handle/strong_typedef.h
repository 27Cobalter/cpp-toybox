#pragma once

template<typename T, typename Tag>
struct StrongTypedef {
  using element_type = T;
  T value;
  explicit constexpr StrongTypedef(T v) : value(v) {}
  explicit StrongTypedef(T* p) : value(p ? *p : T{}) {}
  constexpr const T* AsPtr() const noexcept {
    return &value;
  }
  constexpr T* AsMutPtr() noexcept {
    return &value;
  }
};