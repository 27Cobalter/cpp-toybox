set(CMAKE_CXX_STANDARD 20)
add_library(enum_utils INTERFACE)
target_include_directories(enum_utils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 23)
add_executable(enum_utils_main main.cc)
target_link_libraries(enum_utils_main PRIVATE enum_utils)

add_subdirectory(test)
