find_program(GCOVR_EXECUTABLE gcovr)
find_program(GCOV_EXECUTABLE gcov)

message(STATUS "gcovr executable: ${GCOVR_EXECUTABLE}")
message(STATUS "gcov executable: ${GCOV_EXECUTABLE}")

if(NOT GCOVR_EXECUTABLE OR NOT GCOV_EXECUTABLE)
  message(WARNING "gcov not found")
  return()
endif()

string(
  APPEND
  CMAKE_CXX_FLAGS_DEBUG
  " -O0 -g --coverage -fprofile-abs-path -fprofile-exclude-files=\"build/*;test/*\""
)
string(APPEND CMAKE_EXE_LINKER_FLAGS_DEBUG " --coverage")
string(APPEND CMAKE_SHARED_LINKER_FLAGS_DEBUG " --coverage")
string(APPEND CMAKE_MODULE_LINKER_FLAGS_DEBUG " --coverage")

make_directory("${CMAKE_BINARY_DIR}/coverage")
configure_file(${CMAKE_CURRENT_LIST_DIR}/gcovr.cfg.in gcovr.cfg @ONLY)

message(STATUS "Add Target: coverage")
add_custom_target(
  coverage
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Generate coverage report"
  COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/coverage
  COMMAND ${GCOVR_EXECUTABLE} --config ${CMAKE_BINARY_DIR}/gcovr.cfg
          ${CMAKE_BINARY_DIR})
