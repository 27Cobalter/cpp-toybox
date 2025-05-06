add_library(argument_parser INTERFACE)
target_sources(argument_parser INTERFACE argument_parser.cc)
target_include_directories(argument_parser INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(argument_parser_main main.cc)
target_link_libraries(argument_parser_main PRIVATE argument_parser)

add_subdirectory(test)
