cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(holamundo)

pico_sdk_init()

add_executable(holamundo
        holamundo.c
        )

# pull in common dependencies
target_link_libraries(holamundo pico_stdlib)

# create map/bin/hex file etc.
pico_add_extra_outputs(holamundo)

# Habilita el soporte USB
pico_enable_stdio_usb(holamundo 1)

target_include_directories(holamundo PUBLIC)