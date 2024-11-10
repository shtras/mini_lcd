#pragma once

#include "hagl_hal_color.h"
#include "hagl/window.h"
#include "hagl/bitmap.h"
#include "hagl_hal.h"

#include <hardware/spi.h>

using Pin = int;
class Display
{
public:
    hagl_window_t clip{0, 0, MIPI_DISPLAY_WIDTH - 1, MIPI_DISPLAY_HEIGHT - 1};
    int16_t width = MIPI_DISPLAY_WIDTH;
    int16_t height = MIPI_DISPLAY_HEIGHT;
    uint8_t depth = MIPI_DISPLAY_DEPTH;

    Display(Pin scl, Pin sda, Pin dc, Pin cs, spi_inst_t* spi);

    void init();

    void put_pixel(int16_t x0, int16_t y0, hagl_color_t color);

    void hline(int16_t x0, int16_t y0, uint16_t width, hagl_color_t color);

    void vline(int16_t x0, int16_t y0, uint16_t height, hagl_color_t color);

    void blit(int16_t x0, int16_t y0, hagl_bitmap_t* src);

private:
    void write_command(const uint8_t command);

    void write_data(const uint8_t* data, size_t length);

    void read_data(uint8_t* data, size_t length);

    void set_address_xyxy(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

    void set_address_xy(uint16_t x1, uint16_t y1);

    void spi_master_init();

    size_t fill_xywh(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, void* _color);

    size_t write_xywh(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint8_t* buffer);

    size_t write_xy(uint16_t x1, uint16_t y1, uint8_t* buffer);

    /* TODO: This most likely does not work with dma atm. */
    void ioctl(const uint8_t command, uint8_t* data, size_t size);

    void close();

    Pin scl_ = -1;
    Pin sda_ = -1;
    Pin dc_ = -1;
    Pin cs_ = -1;
    spi_inst_t* spi_ = nullptr;

    uint16_t prev_x1_ = 0;
    uint16_t prev_x2_ = 0;
    uint16_t prev_y1_ = 0;
    uint16_t prev_y2_ = 0;
};