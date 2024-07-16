cmake_minimum_required(VERSION 3.20)

option(USE_VCPKG "USE_VCPKG" OFF)
if(DEFINED ENV{VCPKG_ROOT} AND USE_VCPKG)
	# Use vcpkg
	set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "vcpkg.cmake")
else()
	if(WIN32)
		# Use Local OpenCV
		# Exists OpenCVConfig.cmake dir, and Add $ENV{PATH} to bin dir
		set(OpenCV_DIR "C:/Local/src/opencv-git/build/install")
	endif()
	set(CMAKE_TOOLCHAIN_FILE OFF)
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

add_subdirectory(src)
