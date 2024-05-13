cmake_minimum_required(VERSION 3.20)

project(IMPL_TEST LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

find_package(OpenMP REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

if(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
else()
	add_compile_definitions("/utf-8")
endif()

add_subdirectory(src)
