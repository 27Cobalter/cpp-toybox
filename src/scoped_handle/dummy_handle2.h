#pragma once

#include <cstdint>

#include "scoped_handle.h"
#include "strong_typedef.h"

using DummyHandle2      = int32_t;
using StrongDummyHandle2 = StrongTypedef<DummyHandle2, struct StrongDummyHandleTag2>;

void InitializeHandle2(DummyHandle2** handle);
void ClosoeHandle2(DummyHandle2* handle);

template<>
void ScopedHandleDeleter<StrongDummyHandle2>::operator()(StrongDummyHandle2 p);