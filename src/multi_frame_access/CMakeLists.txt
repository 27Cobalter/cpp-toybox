add_library(multi_frame_access STATIC "multi_frame_access.h"
                                      "multi_frame_access.cc")
target_include_directories(multi_frame_access PUBLIC .)

add_executable(multi_frame_access_main "main.cc")

target_link_libraries(multi_frame_access_main PRIVATE multi_frame_access)
