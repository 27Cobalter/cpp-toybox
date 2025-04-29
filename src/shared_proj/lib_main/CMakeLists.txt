add_executable(lib_main main.cc)
target_compile_definitions(
  lib_main PRIVATE PRINT_CONTEXT="PrintContext defined by lib_main")
target_link_libraries(lib_main PRIVATE lib_shared)
