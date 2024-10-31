cmake_minimum_required(VERSION 3.20)

option(USE_VCPKG "USE_VCPKG" OFF)
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
	if(DEFINED ENV{VCPKG_ROOT} AND USE_VCPKG)
		set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Path to vcpkg.cmake")
	else()
		set(CMAKE_TOOLCHAIN_FILE OFF)
	endif()
endif()

project(IMPL_TEST LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

find_package(OpenMP REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
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

include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_subdirectory(src)
# add_subdirectory(test)
