file(GLOB HISTO_IMPL_SRC histo_impl_*.cc)
add_library(histo "histo.cc" "histo.h" ${HISTO_IMPL_SRC})
target_include_directories(histo PUBLIC .)

add_executable(histo_main "histo_main.cc")
target_link_libraries(histo_main PRIVATE histo)
