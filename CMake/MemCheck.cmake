find_program(VALGRIND_EXECUTABLE valgrind REQUIRED)

if(NOT VALGRIND_EXECUTABLE)
      message(FATAL_ERROR "valgrind not found.")
endif()

message(STATUS "valgrind executable: ${VALGRIND_EXECUTABLE}")
set(MEMORYCHECK_COMMAND "${CMAKE_BINARY_DIR}/valgrind_wrapper.sh")

make_directory(${CMAKE_BINARY_DIR}/memcheck)
make_directory(${CMAKE_BINARY_DIR}/helgrind)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/valgrind_wrapper.sh DESTINATION ${CMAKE_BINARY_DIR}/)

set(CUSTOM_MEMORYCHECK_TYPE "MEMCHECK" CACHE STRING "")
if(CUSTOM_MEMORYCHECK_TYPE STREQUAL "MEMCHECK")
      message(STATUS "Set MemoryCheckType: MEMCHECK")
      set(MEMORYCHECK_COMMAND_OPTIONS "--tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose")
      string(APPEND MEMORYCHECK_COMMAND_OPTIONS " --xml=yes --xml-file=${CMAKE_BINARY_DIR}/memcheck/\${REPLACE_TEST_NAME}.xml")
elseif(CUSTOM_MEMORYCHECK_TYPE STREQUAL "HELGRIND")
      message(STATUS "Set MemoryCheckType: HELGRIND")
      set(MEMORYCHECK_COMMAND_OPTIONS "--tool=helgrind --history-level=full --conflict-cache-size=10000000")
       string(APPEND MEMORYCHECK_COMMAND_OPTIONS " --xml=yes --xml-file=${CMAKE_BINARY_DIR}/helgrind/\${REPLACE_TEST_NAME}.xml")
else()
      message(FATAL_ERROR "CUSTOM_MEMORYCHECK_TYPE Support MEMCHECK or HELGRIND")
endif()
string(APPEND MEMORYCHECK_COMMAND_OPTIONS " --error-limt=no --max-threads=20 --max-stackframe=20000000 --main-stacksize=8000000 --num-callers=20")

include(CTest)
