cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(main)

pico_sdk_init()

add_executable(main
        main.c
        )

# pull in common dependencies
target_link_libraries(main pico_stdlib)
 

# Include the hardware library for PWM
target_link_libraries(main pico_stdlib hardware_pwm)

target_link_libraries(main pico_stdlib hardware_uart)

target_link_libraries(main pico_stdlib hardware_adc)

# Add I2C support
target_link_libraries(main pico_stdlib hardware_i2c)

# create map/bin/hex file etc.
pico_add_extra_outputs(main)

# Habilita el soporte USB
pico_enable_stdio_usb(main 1)
pico_enable_stdio_uart(main 0)

# Enable multicore support
pico_add_extra_outputs(main)


target_link_libraries(main pico_stdlib pico_multicore)


target_include_directories(main PUBLIC)




