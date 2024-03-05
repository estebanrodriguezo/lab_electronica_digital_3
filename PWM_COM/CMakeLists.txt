cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(main2)

pico_sdk_init()

add_executable(main2
        main2.c
        )

# pull in common dependencies
target_link_libraries(main2 pico_stdlib)

# Include the hardware library for PWM
target_link_libraries(main2 pico_stdlib hardware_pwm)

target_link_libraries(main2 pico_stdlib hardware_uart)

target_link_libraries(main2 pico_stdlib hardware_adc)

# create map/bin/hex file etc.
pico_add_extra_outputs(main2)

# Habilita el soporte USB
pico_enable_stdio_usb(main2 1)


target_include_directories(main2 PUBLIC)




