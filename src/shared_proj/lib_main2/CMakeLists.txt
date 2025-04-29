add_executable(lib_main2 main.cc)
target_compile_definitions(
  lib_main2 PRIVATE PRINT_CONTEXT="PrintContext defined by lib_main2")
target_link_libraries(lib_main2 PRIVATE lib_shared)
