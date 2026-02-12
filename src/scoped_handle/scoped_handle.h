#pragma once

#include <memory>

#include <cstdint>
#include <print>

template<typename T>
struct ScopedHandleDeleter {
using pointer = T;
  void operator()(T p) {
    static_assert(false, "ScopedHandleDeleter not specialized for this type");
  }
};

template<typename T>
using UniqueHandle = std::unique_ptr<T, ScopedHandleDeleter<T>>;
