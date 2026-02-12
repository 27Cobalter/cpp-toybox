#include "dummy_handle.h"

void InitializeHandle(DummyHandle** handle) {
  static int32_t counter = 0;
  *handle                = new int32_t(counter++);
  std::println("{}: {}", __func__, **handle);
}

void ClosoeHandle(DummyHandle* handle) {
  std::println("{} : {}", __func__, *handle);
  delete handle;
}

template<>
void ScopedHandleDeleter<StrongDummyHandle>::operator()(StrongDummyHandle p) {
  // ClosoeHandle(p->AsMutPtr());
}
