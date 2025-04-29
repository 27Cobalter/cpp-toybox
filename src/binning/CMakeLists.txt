file(GLOB BINNING_IMPL "binning_impl_*.cc")
file(GLOB BINNING_IMPL_AVX512 "binning_impl_avx512*.cc")
list(REMOVE_ITEM BINNING_IMPL ${BINNING_IMPL_AVX512})

add_library(binning "binning.cc" "binning.h" ${BINNING_IMPL})
target_include_directories(binning PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(binning PRIVATE ${OpenCV_LIBS})
target_include_directories(binning PUBLIC .)

add_library(binning_avx512 STATIC ${BINNING_IMPL_AVX512})
target_include_directories(binning PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(binning PRIVATE ${BINNING_IMPL_AVX512} ${OpenCV_LIBS})
if(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
  target_compile_options(binning_avx512 PRIVATE -mavx512f)
endif()

add_executable(binning_main "main.cc")
target_link_libraries(binning_main PRIVATE binning)

target_include_directories(binning_main PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(binning_main PRIVATE ${OpenCV_LIBS})

add_subdirectory(test)
