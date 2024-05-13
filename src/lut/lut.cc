#include "lut.h"

#include <InstructionInfo.h>

LUT::LUT(int32_t range_max) : range_max_(range_max) {
#ifdef _MSC_VER
  lut_ = std::shared_ptr<uint32_t[]>(new uint32_t[range_max]);
#else
  lut_ = std::shared_ptr<uint32_t[]>(new (std::align_val_t(64)) uint32_t[range_max]);
#endif
  ImplSelector();
}

void LUT::ImplSelector() {
  if (InstructionInfo::IsSupported(InstructionInfo::InstructionSet::AVX512_VBMI)) {
    Create_AutoImpl  = &LUT::Create_Impl<Method::naive_calc>;
    Convert_AutoImpl = &LUT::Convert_Impl<Method::avx512vbmi_calc>;
  } else if (InstructionInfo::IsSupported(InstructionInfo::InstructionSet::AVX512F)) {
    Create_AutoImpl  = &LUT::Create_Impl<Method::naive_calc>;
    Convert_AutoImpl = &LUT::Convert_Impl<Method::avx512f_calc>;
  } else if (InstructionInfo::IsSupported(InstructionInfo::InstructionSet::AVX2)) {
    Create_AutoImpl  = &LUT::Create_Impl<Method::avx2_lut>;
    Convert_AutoImpl = &LUT::Convert_Impl<Method::avx2_lut>;
  } else {
    Create_AutoImpl  = &LUT::Create_Impl<Method::naive_lut>;
    Convert_AutoImpl = &LUT::Convert_Impl<Method::naive_lut>;
  }
}
