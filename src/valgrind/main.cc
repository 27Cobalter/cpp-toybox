#include "cstdint"

#include "valgrind.h"

auto main() -> int32_t {
      MemoryError();
      ThreadError();

      return 0;
}