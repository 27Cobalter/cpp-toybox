if(NOT "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "aarch64")

endif()

add_subdirectory(args)
add_subdirectory(argument_parser)
# add_subdirectory(binning)
add_subdirectory(branch_miss)
add_subdirectory(card)
add_subdirectory(conv)
add_subdirectory(enum_utils)
add_subdirectory(fib)
add_subdirectory(hadd)
# add_subdirectory(hadd_multi)
# add_subdirectory(histo)
add_subdirectory(inter_branch)
add_subdirectory(instruction_info)
# add_subdirectory(lut)
add_subdirectory(magic_enum_call)
add_subdirectory(multi_frame_access)
add_subdirectory(shared_proj)
add_subdirectory(stack_hist)
add_subdirectory(unpack)
# add_subdirectory(vadd)
# add_subdirectory(vhadd)
add_subdirectory(valgrind)

if(NOT WIN32)
add_subdirectory(usb)
endif()
