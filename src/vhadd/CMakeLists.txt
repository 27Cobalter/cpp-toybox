file(GLOB SRC_IMPL "vhadd_impl_*.cc")

add_library(vhadd STATIC "vhadd.cc" "vhadd.h" ${SRC_IMPL})
target_link_libraries(vhadd PRIVATE InstructionInfo)
target_include_directories(vhadd PUBLIC .)

add_executable(vhadd_main "main.cc")

target_include_directories(vhadd_main PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(vhadd_main PRIVATE InstructionInfo ${OpenCV_LIBRARIES})
target_link_libraries(vhadd_main PRIVATE vhadd)
