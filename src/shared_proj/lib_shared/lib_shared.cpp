#include "lib_shared.h"
#include <iostream>

#ifndef PRINT_CONTEXT
#define PRINT_CONTEXT "No Context Defined"
#endif

void PrintHelloWorld() {
  std::cout << PRINT_CONTEXT << std::endl;
}
