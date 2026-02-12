#pragma once

#include <cstdint>

#include "scoped_handle.h"
#include "strong_typedef.h"

using DummyHandle = int32_t;
using StrongDummyHandle = StrongTypedef<DummyHandle, struct StrongDummyHandleTag>;

void InitializeHandle(DummyHandle** handle);
void ClosoeHandle(DummyHandle* handle);

template<>
void ScopedHandleDeleter<StrongDummyHandle>::operator()(StrongDummyHandle p);