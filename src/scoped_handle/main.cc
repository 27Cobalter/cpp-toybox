#include <cstdint>
#include <print>

#include "dummy_handle.h"
#include "dummy_handle2.h"

auto main() -> int32_t {
  // UniqueHandle<double> handle3;

  {
    UniqueHandle<StrongDummyHandle> handle1;
    UniqueHandle<StrongDummyHandle> handle2;
    auto aa = std::out_ptr<StrongDummyHandle::element_type*>(handle1);
    // InitializeHandle(std::out_ptr<StrongDummyHandle::element_type*>(handle1));
    // InitializeHandle(std::out_ptr<StrongDummyHandle::element_type*>(handle2));
    // InitializeHandle(std::out_ptr<StrongDummyHandle::element_type*>(handle1));
  }

  // {
  //   UniqueHandle<DummyHandle2> handle3;
  //   UniqueHandle<DummyHandle2> handle4;
  //   InitializeHandle2(std::out_ptr(handle3));
  //   InitializeHandle2(std::out_ptr(handle4));
  // }

  return 0;
}