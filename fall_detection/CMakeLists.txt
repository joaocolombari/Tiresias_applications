cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(fall_detection)

target_sources(app PRIVATE 
    src/main.c
    src/led/led.c
    src/fall_detection/fall_detection.c
    src/BLE/ble_service.c
)

# NORDIC SDK APP END
zephyr_library_include_directories(.)
