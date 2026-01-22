add_executable(copy_make_border main.cc)

target_include_directories(copy_make_border PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(copy_make_border PRIVATE ${OpenCV_LIBS})