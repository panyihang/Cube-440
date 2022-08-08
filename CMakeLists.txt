cmake_minimum_required(VERSION 3.12)

pico_sdk_init()

include(pico_sdk_import.cmake)

add_executable(usb_microphone
    main.c
    usb_descriptors.c
    usb_microphone.c
    st7789.c
)

target_include_directories(usb_microphone PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(usb_microphone PRIVATE tinyusb_device tinyusb_board pico_pdm_microphone pico_stdlib hardware_spi )

pico_add_extra_outputs(usb_microphone)



