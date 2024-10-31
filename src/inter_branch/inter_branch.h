#pragma once

#include <memory>
#include <cstdint>
#include <span>

class InterBranch {
private:
public:
  void MultiOpInt(bool srli, bool slli, bool add, bool sub, bool bor, bool band, int32_t size,
                  const uint16_t* src, uint16_t* a, uint16_t* b, uint16_t* c, uint16_t* d,
                  uint16_t* e, uint16_t* f);
  void MultiOpExt(bool srli, bool slli, bool add, bool sub, bool bor, bool band, int32_t size,
                  const uint16_t* src, uint16_t* a, uint16_t* b, uint16_t* c, uint16_t* d,
                  uint16_t* e, uint16_t* f);
};
