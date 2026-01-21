add_executable(alloc_initializer main.cc)
target_include_directories(alloc_initializer PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(alloc_initializer PRIVATE binning ${OpenCV_LIBS})