file(GLOB TEST_SOURCE "*.cc")
add_executable(test_argument_parser ${TEST_SOURCE})
target_link_libraries(test_argument_parser PRIVATE argument_parser)

include(GoogleTest)

target_link_libraries(test_argument_parser PRIVATE GTest::gtest_main enum_utils)
gtest_discover_tests(test_argument_parser)
