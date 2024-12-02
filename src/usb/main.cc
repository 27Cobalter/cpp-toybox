#include <iostream>
#include <span>
#include <cstdint>
#include <print>

#include <libusb-1.0/libusb.h>

auto main() -> int {
  libusb_device** list;

  libusb_init(nullptr);

  int cnt = libusb_get_device_list(nullptr, &list);
  std::cout << cnt << std::endl;

  char text[512];

  std::span<libusb_device*> devices{list, cnt};
  for (auto& device : devices) {
    struct libusb_device_descriptor desc;
    libusb_device_handle* handle;

    libusb_get_device_descriptor(device, &desc);

    auto ret = libusb_open(device, &handle);
    if (ret == 0) {
      std::println("{:16s} : {:04x}", "idVendor", desc.idVendor);
      std::println("{:16s} : {:04x}", "idProduct", desc.idProduct);

      libusb_get_string_descriptor_ascii(handle, desc.iManufacturer,
                                         reinterpret_cast<unsigned char*>(text), sizeof(text));
      std::println("{:16s}: {}", "iManufacturer", text);
      libusb_get_string_descriptor_ascii(handle, desc.iProduct,
                                         reinterpret_cast<unsigned char*>(text), sizeof(text));
      std::println("{:16s}: {}", "iProduct", text);

      std::println();
    }
  }
  return 0;
}
