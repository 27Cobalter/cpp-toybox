file(GLOB HISTO_IMPL_SRC histo_impl_*.cc)
add_library(histo "histo.cc" "histo.h" ${HISTO_IMPL_SRC})
target_include_directories(histo PUBLIC .)

find_package(OpenCV REQUIRED)

add_executable(histo_main "histo_main.cc")
target_link_libraries(histo_main PRIVATE histo)

target_include_directories(histo_main PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(histo_main PRIVATE ${OpenCV_LIBS})
