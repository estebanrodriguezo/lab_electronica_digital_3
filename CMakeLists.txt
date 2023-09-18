cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(main)

pico_sdk_init()

add_executable(main
        main.c
        )

# pull in common dependencies
target_link_libraries(main pico_stdlib)

# create map/bin/hex file etc.
pico_add_extra_outputs(main)

# Habilita el soporte USB
pico_enable_stdio_usb(main 1)

target_include_directories(main PUBLIC)