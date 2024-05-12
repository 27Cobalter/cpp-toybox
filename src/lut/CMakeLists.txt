add_executable(lut_process "main.cc" "lut.cc" "lut.h")
target_link_libraries(lut_process PRIVATE InstructionInfo)
