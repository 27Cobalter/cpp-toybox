file(GLOB TEST_SOURCE test_*.cc)
add_executable(test_enum_operator ${TEST_SOURCE})

include(GoogleTest)
target_link_libraries(test_enum_operator PRIVATE GTest::gtest_main)
gtest_discover_tests(test_enum_operator)
