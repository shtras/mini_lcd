cmake_minimum_required(VERSION 3.24)
include(pico_sdk_import.cmake)
project(MiniLcd C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

pico_sdk_init()

add_subdirectory(hagl)
add_subdirectory(hagl_hal)

add_executable(mini_lcd
        main.cpp
)

# pull in common dependencies
target_link_libraries(mini_lcd pico_stdlib hagl hagl_hal)

target_compile_definitions(mini_lcd PRIVATE
        HAGL_HAL_USE_SINGLE_BUFFER
)

target_compile_definitions(mini_lcd PRIVATE
    #MIPI_DISPLAY_PIN_DC=14
    #MIPI_DISPLAY_PIN_CS=13
    #MIPI_DISPLAY_PIN_CLK=10
    #MIPI_DISPLAY_PIN_MOSI=11
    #MIPI_DISPLAY_PIN_RST=15
    MIPI_DISPLAY_PIN_RST=-1
    #MIPI_DISPLAY_PIN_BL=9
    MIPI_DISPLAY_PIN_BL=-1
    MIPI_DISPLAY_PIN_MISO=-1
    MIPI_DISPLAY_PIN_POWER=-1
    MIPI_DISPLAY_PIN_TE=-1

    #MIPI_DISPLAY_SPI_PORT=spi1
    MIPI_DISPLAY_SPI_CLOCK_SPEED_HZ=62500000
    MIPI_DISPLAY_PIXEL_FORMAT=MIPI_DCS_PIXEL_FORMAT_16BIT

    MIPI_DISPLAY_ADDRESS_MODE=MIPI_DCS_ADDRESS_MODE_RGB|MIPI_DCS_ADDRESS_MODE_MIRROR_Y|MIPI_DCS_ADDRESS_MODE_MIRROR_X
    MIPI_DISPLAY_WIDTH=128
    MIPI_DISPLAY_HEIGHT=160
    MIPI_DISPLAY_OFFSET_X=0
    MIPI_DISPLAY_OFFSET_Y=0

    # MIPI_DISPLAY_ADDRESS_MODE=MIPI_DCS_ADDRESS_MODE_BGR|MIPI_DCS_ADDRESS_MODE_SWAP_XY|MIPI_DCS_ADDRESS_MODE_MIRROR_Y
    # MIPI_DISPLAY_WIDTH=128
    # MIPI_DISPLAY_HEIGHT=128
    # MIPI_DISPLAY_OFFSET_X=2
    # MIPI_DISPLAY_OFFSET_Y=2

    MIPI_DISPLAY_INVERT=0
    HAGL_HAL_DEBUG=1
)

# create map/bin/hex file etc.
pico_add_extra_outputs(mini_lcd)

# enable usb output, disable uart output
pico_enable_stdio_usb(mini_lcd 1)
pico_enable_stdio_uart(mini_lcd 0)