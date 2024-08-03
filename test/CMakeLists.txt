if(NOT "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "aarch64")
  add_subdirectory(test_vhadd)
endif()
