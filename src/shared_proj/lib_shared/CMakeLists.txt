add_library(lib_shared INTERFACE)
target_sources(lib_shared INTERFACE lib_shared.cpp)
target_include_directories(lib_shared INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# add_library(lib_shared STATIC) target_sources(lib_shared PRIVATE
# lib_shared.cpp) target_include_directories(lib_shared PUBLIC
# ${CMAKE_CURRENT_SOURCE_DIR})
