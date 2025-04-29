add_library(inter_branch STATIC inter_branch.cc inter_branch.h)
target_include_directories(inter_branch PUBLIC .)

add_executable(inter_branch_main main.cc)
target_link_libraries(inter_branch_main PRIVATE inter_branch)
