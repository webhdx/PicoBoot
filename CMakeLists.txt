cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(cmake/extract_version.cmake)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(picoboot LANGUAGES C CXX ASM VERSION ${PROJECT_VERSION_STRING})

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist)

add_executable(picoboot
        src/picoboot.c
        src/hw.c
        src/pio.c
        src/status_led.c
        src/status_led/gpio.c
        src/status_led/cyw43.c
)

pico_generate_pio_header(picoboot  
        ${CMAKE_CURRENT_LIST_DIR}/src/picoboot.pio
)

pico_set_program_name(picoboot "PicoBoot")
pico_set_program_description(picoboot "RP2040 based modchip for Nintendo GameCube")
pico_set_program_version(picoboot ${FW_VER_STRING})
pico_set_program_url(picoboot "https://github.com/webhdx/PicoBoot")

target_link_options(pico_standard_link INTERFACE "LINKER:--script=${CMAKE_CURRENT_LIST_DIR}/memmap_picoboot.ld")

pico_enable_stdio_uart(picoboot 0)
pico_enable_stdio_usb(picoboot 1)

target_include_directories(picoboot PRIVATE src)

target_link_libraries(picoboot PRIVATE 
        hardware_adc
        hardware_dma
        hardware_pio
        pico_cyw43_arch_none
        pico_stdlib
)

pico_add_extra_outputs(picoboot)
