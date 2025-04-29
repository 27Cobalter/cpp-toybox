option(BULID_INSTRUCTION_TEST "BUILD instruction_info Test" OFF)

add_library(instruction_info STATIC "instruction_info.cc" "instruction_info.h")
target_include_directories(instruction_info PUBLIC .)

if(BULID_INSTRUCTION_TEST)
  add_executable(instruction_test main.cc)
  target_link_libraries(instruction_test PRIVATE instruction_info)
endif()
