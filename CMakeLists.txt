cmake_minimum_required(VERSION 3.24)
include(pico_sdk_import.cmake)
project(MiniLcd C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

pico_sdk_init()

add_subdirectory(hagl)

add_library(mini_lcd_compiler_flags INTERFACE)
target_compile_features(mini_lcd_compiler_flags INTERFACE cxx_std_20)
target_compile_options(mini_lcd_compiler_flags INTERFACE -Wall -Wextra)
target_compile_options(mini_lcd_compiler_flags INTERFACE "$<$<CONFIG:Debug>:-Wno-unused-function;-Wno-unused-parameter;-Wno-unused-variable>")
target_compile_options(mini_lcd_compiler_flags INTERFACE "$<$<NOT:$<CONFIG:Debug>>:-Werror>")

find_package(nlohmann_json REQUIRED)

add_executable(mini_lcd
        Encoder.cpp
        Snake.cpp
        Utils.cpp
        TCP.cpp
        Comm.cpp
        tiny-json.c
        main.cpp
)

target_compile_definitions(mini_lcd PRIVATE
        HAGL_HAL_USE_SINGLE_BUFFER
)

target_include_directories(mini_lcd PRIVATE
        ${CMAKE_CURRENT_LIST_DIR})

target_compile_definitions(mini_lcd PRIVATE
        MIPI_DISPLAY_PIN_RST=-1

        MIPI_DISPLAY_PIN_BL=-1
        MIPI_DISPLAY_PIN_MISO=-1
        MIPI_DISPLAY_PIN_POWER=-1
        MIPI_DISPLAY_PIN_TE=-1

        MIPI_DISPLAY_SPI_CLOCK_SPEED_HZ=62500000
        MIPI_DISPLAY_PIXEL_FORMAT=MIPI_DCS_PIXEL_FORMAT_16BIT

        MIPI_DISPLAY_ADDRESS_MODE=MIPI_DCS_ADDRESS_MODE_RGB|MIPI_DCS_ADDRESS_MODE_MIRROR_Y|MIPI_DCS_ADDRESS_MODE_MIRROR_X
        MIPI_DISPLAY_WIDTH=128
        MIPI_DISPLAY_HEIGHT=160
        MIPI_DISPLAY_OFFSET_X=0
        MIPI_DISPLAY_OFFSET_Y=0

        MIPI_DISPLAY_INVERT=0
        HAGL_HAL_DEBUG=1

        WIFI_SSID="${WIFI_SSID}"
        WIFI_PASSWORD="${WIFI_PASSWORD}"
        SERVER_ADDR="${SERVER_ADDR}"
)

target_link_libraries(mini_lcd mini_lcd_compiler_flags pico_stdlib hagl pico_multicore pico_cyw43_arch_lwip_threadsafe_background)

# create map/bin/hex file etc.
pico_add_extra_outputs(mini_lcd)

# enable usb output, disable uart output
pico_enable_stdio_usb(mini_lcd 1)
pico_enable_stdio_uart(mini_lcd 0)