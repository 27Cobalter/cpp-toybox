add_executable(test_gtest test.cc)

include(GoogleTest)
target_link_libraries(test_gtest PRIVATE GTest::gtest_main)
gtest_discover_tests(test_gtest)
