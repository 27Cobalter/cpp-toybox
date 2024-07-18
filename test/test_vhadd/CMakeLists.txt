add_executable(test_vhadd test_vhadd_naive.cc test_vhadd_avx2.cc "misc.h")


include(GoogleTest)

target_include_directories(test_vhadd PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(test_vhadd PRIVATE vhadd InstructionInfo ${OpenCV_LIBRARIES} GTest::gtest_main)

gtest_discover_tests(test_vhadd)