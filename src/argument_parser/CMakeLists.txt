add_library(ArgumentParser INTERFACE)
target_sources(ArgumentParser INTERFACE argument_parser.cc)
target_include_directories(ArgumentParser INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(ArgumentParserMain main.cc)
target_link_libraries(ArgumentParserMain PRIVATE ArgumentParser)
