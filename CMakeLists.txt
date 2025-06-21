cmake_minimum_required(VERSION 3.20)

option(USE_VCPKG "USE_VCPKG" OFF)
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VCPKG_ROOT} AND USE_VCPKG)
    set(CMAKE_TOOLCHAIN_FILE
        "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Path to vcpkg.cmake")
  else()
    set(CMAKE_TOOLCHAIN_FILE OFF)
  endif()
endif()

if(NOT CMAKE_C_COMPILER)
  set(CMAKE_C_COMPILER gcc)
endif()
if(NOT CMAKE_CXX_COMPILER)
  set(CMAKE_CXX_COMPILER g++)
endif()
project(IMPL_TEST LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

# find_program(CLANG_TIDY_EXECUTABLE clang-tidy REQUIRED)
# set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXECUTABLE})

find_package(OpenMP REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

if(CMAKE_CXX_COMPILER EQUAL "clang++")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif()

if(UNIX)
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=x86-64-v3")
endif()

find_package(OpenCV)
if(NOT OpenCV_FOUND)
  if(DEFINED ENV{OPENCV_ROOT})
    set(OpenCV_DIR $ENV{OPENCV_ROOT})
  elseif(WIN32)
    set(OpenCV_DIR "C:/Local/src/opencv-git/build/install")
  else()
    set(OpenCV_DIR "~/.local/src/opencv-git/build/install")
  endif()
  find_package(OpenCV REQUIRED)
endif()

if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip)
set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Coverage
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  include(CMake/Coverage.cmake)
  include(CMake/MemCheck.cmake)
endif()

add_subdirectory(src)
