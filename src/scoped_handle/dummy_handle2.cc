#include "dummy_handle2.h"

void InitializeHandle2(DummyHandle2** handle) {
  static int32_t counter = 0;
  *handle                = new int32_t(counter++);
  std::println("{}: {}", __func__, **handle);
}

void ClosoeHandle2(DummyHandle2* handle) {
  std::println("{} : {}", __func__, *handle);
  delete handle;
}

template<>
void ScopedHandleDeleter<StrongDummyHandle2>::operator()(StrongDummyHandle2 p) {
  // ClosoeHandle2(p->AsMutPtr());
}
