file(GLOB TEST_SOURCE "*.cc")
add_executable(test_enum_utils ${TEST_SOURCE})
target_link_libraries(test_enum_utils PRIVATE enum_utils)

include(GoogleTest)

target_link_libraries(test_enum_utils PRIVATE enum_utils GTest::gtest_main)

gtest_discover_tests(test_enum_utils)
