add_executable(branch_miss main.cc)
if(UNIX)
  target_compile_options(branch_miss PRIVATE "-fno-inline")
endif()
