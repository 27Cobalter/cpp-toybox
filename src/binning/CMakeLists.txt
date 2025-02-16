file(GLOB BINNING_IMPL "binning_impl_*.cc")
add_library(binning "binning.cc" "binning.h" ${BINNING_IMPL})
target_include_directories(binning PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(binning PRIVATE ${OpenCV_LIBS})
target_include_directories(binning PUBLIC .)

add_executable(binning_main "binning_main.cc")
target_link_libraries(binning_main PRIVATE binning)

target_include_directories(binning_main PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(binning_main PRIVATE ${OpenCV_LIBS})

add_subdirectory(test)
