add_executable(aligned_sum main.cc)
target_compile_options(aligned_sum PRIVATE -mavx2 -mfma -march=native)