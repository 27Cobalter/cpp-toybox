#pragma once

#include <array>
#include <cstdint>
#include <ostream>

class InstructionInfo {
private:
  static std::array<int32_t, 4> f1_;
  static std::array<int32_t, 4> f7_;
  static std::array<int32_t, 4> f7e1_;
  void CpuId(std::array<int32_t, 4>& reg, int32_t i);
  void CpuIdex(std::array<int32_t, 4>& reg, int32_t i, int32_t c);

public:
  enum class InstructionSet {
    // __cpuid_count(FeatureID, FeatureID_extention, eax, ebx, ecx, edx)
    // 0x00FF0000 FeatureID
    // 0x00003000 FeatureID_extention
    // 0x00000300 Register 0: eax, 1: ebx, 2: ecx, 3, edx
    // 0x0000001F Infomation Bit
    //
    // intel-architecture-instruction-set-extensions-programming-reference
    // 1.4 DETECTION OF FEATURE INSTRUCTIONS AND FEATURES

    // AVX: Vector
    AVX                  = 0x00010200 | 28,
    AVX2                 = 0x00070100 | 5,
    AVX512F              = 0x00070100 | 16,
    AVX512DQ             = 0x00070100 | 17,
    AVX512_IFMA          = 0x00070100 | 21,
    AVX512PF             = 0x00070100 | 26,
    AVX512ER             = 0x00070100 | 27,
    AVX512CD             = 0x00070100 | 28,
    AVX512BW             = 0x00070100 | 30,
    AVX512VL             = 0x00070100 | 31,
    AVX512_VBMI          = 0x00070200 | 1,
    AVX512_VBMI2         = 0x00070200 | 6,
    AVX512_VNNI          = 0x00070200 | 11,
    AVX512_BITALG        = 0x00070200 | 12,
    AVX512_VPOPCNTDQ     = 0x00070200 | 14,
    AVX512_4VNNIW        = 0x00070300 | 2,
    AVX512_4FMAPS        = 0x00070300 | 3,
    AVX512_VP2INTERESECT = 0x00070300 | 8,
    AVX512_FP16          = 0x00070300 | 23,
    AVX_VNNI             = 0x00071000 | 4,
    AVX512_BF16          = 0x00071000 | 5,
    AVX_IFMA             = 0x00071000 | 23,
    AVX_VNNI_INT8        = 0x00071300 | 4,
    AVX_NE_CONVERT       = 0x00071300 | 5,
    AVX_VNNI_INT16       = 0x00071300 | 10,
    AMX_BF16             = 0x00070300 | 22,
    AMX_TILE             = 0x00070300 | 24,
    AMX_INT8             = 0x00070300 | 25,
    AMX_FP16             = 0x00071000 | 21,
    AMX_COMPLEX          = 0x00071300 | 8,
  };

public:
  InstructionInfo();
  static bool IsSupported(InstructionSet id);
  static void Display(std::ostream& os);
};
