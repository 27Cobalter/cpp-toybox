file(GLOB TEST_SOURCE test_*.cc)
add_executable(test_vhadd ${TEST_SOURCE})

include(GoogleTest)

target_include_directories(test_vhadd PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(test_vhadd PRIVATE vhadd InstructionInfo
                                         ${OpenCV_LIBRARIES} GTest::gtest_main)

gtest_discover_tests(test_vhadd)
