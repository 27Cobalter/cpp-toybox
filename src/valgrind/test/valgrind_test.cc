#include <gtest/gtest.h>

#include "valgrind.h"

TEST(Valgrind, Memcheck) {
  MemoryError();
}

TEST(Valgrind, Helgrind) {
  ThreadError();
}