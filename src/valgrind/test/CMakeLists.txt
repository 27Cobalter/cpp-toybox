set(project_name "valgrind")
file(GLOB TEST_SOURCE "*.cc")
add_executable(test_${project_name} ${TEST_SOURCE})
target_link_libraries(test_${project_name} PRIVATE ${project_name})

include(GoogleTest)

target_link_libraries(test_${project_name} PRIVATE GTest::gtest_main)
gtest_discover_tests(test_${project_name})

get_property(ALL_TEST_NAME GLOBAL PROPERTY TESTS)

message(STATUS "All TESTS: ${ALL_TEST_NAME}")