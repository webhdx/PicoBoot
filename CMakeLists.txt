cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

find_package(Git)
execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --always --dirty
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	OUTPUT_VARIABLE GIT_REPO_VERSION
	OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REGEX REPLACE "v([0-9]+\\.[0-9]+).*" "\\1" CMAKE_GIT_REPO_VERSION ${GIT_REPO_VERSION})
string(REGEX REPLACE "^(.......-.*)|(.......)$" "0.0.0" CMAKE_GIT_REPO_VERSION ${CMAKE_GIT_REPO_VERSION})
configure_file("src/version.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/src/version.h")
message("GIT_REPO_VERSION is ${GIT_REPO_VERSION}")
message("CMAKE_GIT_REPO_VERSION is ${CMAKE_GIT_REPO_VERSION}")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(picoboot LANGUAGES C CXX ASM VERSION ${CMAKE_GIT_REPO_VERSION})

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist)

add_executable(picoboot
        src/picoboot.c
        src/pio.c
)

pico_generate_pio_header(picoboot  
        ${CMAKE_CURRENT_LIST_DIR}/src/picoboot.pio
)

pico_set_program_name(picoboot "PicoBoot")
pico_set_program_description(picoboot "RP2040 based modchip for Nintendo GameCube")
pico_set_program_version(picoboot ${GIT_REPO_VERSION})
pico_set_program_url(picoboot "https://github.com/webhdx/PicoBoot")

pico_set_binary_type(picoboot copy_to_ram)
target_link_options(pico_standard_link INTERFACE "LINKER:--script=${CMAKE_CURRENT_LIST_DIR}/memmap_picoboot.ld")

pico_enable_stdio_uart(picoboot 0)
pico_enable_stdio_usb(picoboot 1)

target_include_directories(picoboot PRIVATE src)

target_link_libraries(picoboot PRIVATE pico_stdlib hardware_pio hardware_dma)

pico_add_extra_outputs(picoboot)

function(merge_uf2 NAME BASE_TARGET INPUTS)
        get_target_property(BASE_TARGET_NAME ${BASE_TARGET} OUTPUT_NAME)
        if(BASE_TARGET_NAME STREQUAL "BASE_TARGET_NAME-NOTFOUND")
                get_target_property(BASE_TARGET_NAME ${BASE_TARGET} NAME)
        endif()

        add_custom_target(${NAME} ALL
                COMMAND ${CMAKE_CURRENT_LIST_DIR}/merge_uf2.py ${NAME}.uf2 ${BASE_TARGET_NAME}.uf2 ${INPUTS}
                DEPENDS ${BASE_TARGET} ${INPUTS}
                COMMAND_EXPAND_LISTS)
endfunction()
