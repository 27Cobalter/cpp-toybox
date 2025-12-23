add_executable(usb_main "main.cc")

find_package(PkgConfig)
pkg_check_modules(LIBUSB_1_0 REQUIRED libusb-1.0)

target_include_directories(usb_main PRIVATE ${LIBUSB_1_0_INCLUDE_DIRS})
target_link_libraries(usb_main PRIVATE ${LIBUSB_1_0_LIBRARIES})
