cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(SHAKE256)

pico_sdk_init()

add_executable(SHAKE256
        SHAKE256.c
        )

# pull in common dependencies
target_link_libraries(SHAKE256 pico_stdlib)

# create map/bin/hex file etc.
pico_add_extra_outputs(SHAKE256)

# Habilita el soporte USB
pico_enable_stdio_usb(SHAKE256 1)

target_include_directories(SHAKE256 PUBLIC)