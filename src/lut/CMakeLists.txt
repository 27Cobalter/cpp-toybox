add_executable(
  lut_process
  "main.cc"
  "lut.cc"
  "lut.h"
  "lut_impl_naive.cc"
  "lut_impl_avx2.cc"
  "lut_impl_avx512f.cc"
  "lut_impl_avx512vbmi.cc")
if(WIN32)
  set_target_properties(lut_process PROPERTIES LINK_FLAGS "/PROFILE")
endif()
target_link_libraries(lut_process PRIVATE InstructionInfo)
