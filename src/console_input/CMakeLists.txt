add_library(console_input console_input.cc)
add_executable(console_input_main main.cc)

target_link_libraries(console_input_main PRIVATE console_input enum_utils)
