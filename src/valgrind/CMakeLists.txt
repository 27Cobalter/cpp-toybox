add_library(valgrind INTERFACE)
target_sources(valgrind INTERFACE valgrind.cc)
target_include_directories(valgrind INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(valgrind_main main.cc)
target_link_libraries(valgrind_main PRIVATE valgrind)

add_subdirectory(test)
