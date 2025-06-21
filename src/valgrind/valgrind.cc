#include "valgrind.h"

#include <print>
#include <thread>
#include <vector>
#include <ranges>
#include <cstdint>

void MemoryError()
{
      int32_t a;
      int32_t* ptr = new int[32];
      ptr[0] = 32;

      ptr[1] = a;

      std::println("{} {} {}", a, ptr[0], ptr[1]);
}

void ThreadError()
{
      int32_t a = 0;

      std::vector<std::thread> vec;

      for(auto i : std::views::iota(0 ,10))
      {
            vec.emplace_back([&a](){
                  a = a + 1;
                  a = a + 1;
            });
      }

      for(auto& th : vec)
      {
            th.join();
      }

      std::println("{}", a);
}