add_library(argument_parser INTERFACE)
target_sources(argument_parser INTERFACE argument_parser.cc)
target_include_directories(argument_parser INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(argument_parserMain main.cc)
target_link_libraries(argument_parserMain PRIVATE argument_parser)

add_subdirectory(test)
