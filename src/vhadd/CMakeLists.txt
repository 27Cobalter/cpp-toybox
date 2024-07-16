file(GLOB SRC_IMPL "vhadd_impl_*.cc")

add_executable(vhadd "main.cc" "vhadd.cc" "vhadd.h" ${SRC_IMPL})

target_include_directories(vhadd PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(vhadd PRIVATE ${OpenCV_LIBRARIES})
target_link_libraries(vhadd PRIVATE InstructionInfo)
