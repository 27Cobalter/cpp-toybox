option(BULID_INSTRUCTION_TEST "BUILD InstructionInfo Test" OFF)

add_library(InstructionInfo STATIC "InstructionInfo.cc" "InstructionInfo.h")
target_include_directories(InstructionInfo PUBLIC .)

if(BULID_INSTRUCTION_TEST)
  add_executable(instruction_test main.cc)
  target_link_libraries(instruction_test PRIVATE InstructionInfo)
endif()
