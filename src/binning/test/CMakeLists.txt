file(GLOB TEST_SOURCE "*.cc")
add_executable(test_binning ${TEST_SOURCE})
target_link_libraries(test_binning PRIVATE binning)

include(GoogleTest)

target_include_directories(test_binning PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(test_binning PRIVATE ${OpenCV_LIBS} binning GTest::gtest_main)

gtest_discover_tests(test_binning)
