#include "instruction_info.h"

#include <format>

#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__)
#include <cpuid.h>
#else
static_assert(false);
#endif

InstructionInfo instructioninfo;
std::array<int32_t, 4> InstructionInfo::f1_;
std::array<int32_t, 4> InstructionInfo::f7_;
std::array<int32_t, 4> InstructionInfo::f7e1_;

void InstructionInfo::CpuId(std::array<int32_t, 4>& reg, int32_t i) {
#if defined(_MSC_VER)
  __cpuid(reinterpret_cast<int32_t*>(&reg), i);
#else
  __cpuid(i, reg[0], reg[1], reg[2], reg[3]);
#endif
}
void InstructionInfo::CpuIdex(std::array<int32_t, 4>& reg, int32_t i, int32_t c) {
#if defined(_MSC_VER)
  __cpuidex(reinterpret_cast<int32_t*>(&reg), i, c);
#else
  __cpuid_count(i, c, reg[0], reg[1], reg[2], reg[3]);
#endif
}

InstructionInfo::InstructionInfo() {
  CpuId(f1_, 1);
  CpuIdex(f7_, 7, 0);
  CpuIdex(f7e1_, 7, 1);
}

bool InstructionInfo::IsSupported(InstructionInfo::InstructionSet id) {
  int32_t feature_id  = (static_cast<int32_t>(id) & 0x0F0000) >> 16;
  int32_t register_id = (static_cast<int32_t>(id) & 0x0300) >> 8;
  int32_t target_bit  = static_cast<int32_t>(id) & 0x1F;
  if (feature_id == 1) {
    return ((f1_[register_id] >> target_bit) & 1) == 1;
  } else if (feature_id == 7) {
    return ((f7_[register_id] >> target_bit) & 1) == 1;
  }
  return false;
}

void InstructionInfo::Display(std::ostream& os) {
#define DISPLAY_INSTRUCTION(x) \
  os << std::format("{:20} : {}", #x, IsSupported(InstructionSet::x)) << std::endl;
  DISPLAY_INSTRUCTION(AVX2);
  DISPLAY_INSTRUCTION(AVX512F);
  DISPLAY_INSTRUCTION(AVX512DQ);
  DISPLAY_INSTRUCTION(AVX512_IFMA);
  DISPLAY_INSTRUCTION(AVX512PF);
  DISPLAY_INSTRUCTION(AVX512ER);
  DISPLAY_INSTRUCTION(AVX512CD);
  DISPLAY_INSTRUCTION(AVX512BW);
  DISPLAY_INSTRUCTION(AVX512VL);
  DISPLAY_INSTRUCTION(AVX512_VBMI);
  DISPLAY_INSTRUCTION(AVX512_VBMI2);
  DISPLAY_INSTRUCTION(AVX512_VNNI);
  DISPLAY_INSTRUCTION(AVX512_BITALG);
  DISPLAY_INSTRUCTION(AVX512_VPOPCNTDQ);
  DISPLAY_INSTRUCTION(AVX512_4VNNIW);
  DISPLAY_INSTRUCTION(AVX512_4FMAPS);
  DISPLAY_INSTRUCTION(AVX512_VP2INTERESECT);
  DISPLAY_INSTRUCTION(AVX512_FP16);
  DISPLAY_INSTRUCTION(AVX_VNNI);
  DISPLAY_INSTRUCTION(AVX512_BF16);
  DISPLAY_INSTRUCTION(AVX_IFMA);
  DISPLAY_INSTRUCTION(AVX_VNNI_INT8);
  DISPLAY_INSTRUCTION(AVX_NE_CONVERT);
  DISPLAY_INSTRUCTION(AVX_VNNI_INT16);
  DISPLAY_INSTRUCTION(AMX_BF16);
  DISPLAY_INSTRUCTION(AMX_TILE);
  DISPLAY_INSTRUCTION(AMX_INT8);
  DISPLAY_INSTRUCTION(AMX_FP16);
  DISPLAY_INSTRUCTION(AMX_COMPLEX);
#undef DISPLAY_INSTRUCTION
}
