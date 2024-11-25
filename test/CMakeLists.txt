if(NOT "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "aarch64")
  add_subdirectory(test_vhadd)
endif()

add_subdirectory(enum_operator)
