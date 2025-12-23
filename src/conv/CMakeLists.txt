add_executable(conv_main main.cc)

find_package(OpenCV REQUIRED)

target_include_directories(conv_main PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(conv_main PRIVATE ${OpenCV_LIBS})
