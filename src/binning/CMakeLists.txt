file(GLOB BINNING_IMPL "binning_impl_*.cc")
file(GLOB BINNING_IMPL_AVX512 "binning_impl_avx512*.cc")

add_library(binning "binning.cc" "binning.h" ${BINNING_IMPL})
target_include_directories(binning PUBLIC .)
target_include_directories(binning PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(binning PRIVATE ${OpenCV_LIBS})

add_executable(binning_main "main.cc")
target_include_directories(binning_main PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(binning_main PRIVATE binning ${OpenCV_LIBS})

add_subdirectory(test)
