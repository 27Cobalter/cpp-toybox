add_executable(magic_enum_call main.cc)

include(FetchContent)
FetchContent_Declare(
  magic_enum
  GIT_REPOSITORY https://github.com/Neargye/magic_enum
  GIT_TAG v0.9.7)
FetchContent_MakeAvailable(magic_enum)

target_link_libraries(magic_enum_call magic_enum::magic_enum)
