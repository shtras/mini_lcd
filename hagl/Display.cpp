/*

MIT License

Copyright (c) 2019-2023 Mika Tuupola

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-cut-

This file is part of the Raspberry Pi Pico MIPI DCS backend for the HAGL
graphics library: https://github.com/tuupola/hagl_pico_mipi

SPDX-License-Identifier: MIT

*/

#include "Display.h"

#include "hagl_hal.h"
#include "mipi_dcs.h"

#include "hagl.h"

#include <hardware/spi.h>
#include <hardware/dma.h>
#include <hardware/gpio.h>
#include <hardware/clocks.h>
#include <cstdio>
#include <cstdlib>
#include <pico/time.h>

static inline uint16_t htons(uint16_t i)
{
    __asm("rev16 %0, %0" : "+l"(i) : :);
    return i;
}

Display::Display(Pin scl, Pin sda, Pin dc, Pin cs, spi_inst_t* spi)
    : scl_(scl)
    , sda_(sda)
    , dc_(dc)
    , cs_(cs)
    , spi_(spi)
{
    gpio_set_function(dc, GPIO_FUNC_SIO);
    gpio_set_dir(dc, GPIO_OUT);
    gpio_set_function(cs, GPIO_FUNC_SIO);
    gpio_set_dir(cs, GPIO_OUT);

    gpio_set_function(scl, GPIO_FUNC_SPI);
    gpio_set_function(sda, GPIO_FUNC_SPI);
    gpio_put(cs, 1);
}

void Display::write_command(const uint8_t command)
{
    /* Set DC low to denote incoming command. */
    gpio_put(dc_, 0);

    /* Set CS low to reserve the SPI bus. */
    gpio_put(cs_, 0);

    spi_write_blocking(spi_, &command, 1);

    /* Set CS high to ignore any traffic on SPI bus. */
    gpio_put(cs_, 1);
}

void Display::write_data(const uint8_t* data, size_t length)
{
    size_t sent = 0;

    if (0 == length) {
        return;
    };

    /* Set DC high to denote incoming data. */
    gpio_put(dc_, 1);

    /* Set CS low to reserve the SPI bus. */
    gpio_put(cs_, 0);

    for (size_t i = 0; i < length; ++i) {
        while (!spi_is_writable(spi_)) {
        };
        spi_get_hw(spi_)->dr = (uint32_t)data[i];
    }

    /* Wait for shifting to finish. */
    while (spi_get_hw(spi_)->sr & SPI_SSPSR_BSY_BITS) {
    };
    spi_get_hw(spi_)->icr = SPI_SSPICR_RORIC_BITS;

    /* Set CS high to ignore any traffic on SPI bus. */
    gpio_put(cs_, 1);
}

void Display::read_data(uint8_t* data, size_t length)
{
    if (0 == length) {
        return;
    };
}

void Display::set_address_xyxy(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint8_t command;
    uint8_t data[4];
    //static uint16_t prev_x1, prev_x2, prev_y1, prev_y2;

    x1 = x1 + MIPI_DISPLAY_OFFSET_X;
    y1 = y1 + MIPI_DISPLAY_OFFSET_Y;
    x2 = x2 + MIPI_DISPLAY_OFFSET_X;
    y2 = y2 + MIPI_DISPLAY_OFFSET_Y;

    /* Change column address only if it has changed. */
    if ((prev_x1_ != x1 || prev_x2_ != x2)) {
        write_command(MIPI_DCS_SET_COLUMN_ADDRESS);
        data[0] = x1 >> 8;
        data[1] = x1 & 0xff;
        data[2] = x2 >> 8;
        data[3] = x2 & 0xff;
        write_data(data, 4);

        prev_x1_ = x1;
        prev_x2_ = x2;
    }

    /* Change page address only if it has changed. */
    if ((prev_y1_ != y1 || prev_y2_ != y2)) {
        write_command(MIPI_DCS_SET_PAGE_ADDRESS);
        data[0] = y1 >> 8;
        data[1] = y1 & 0xff;
        data[2] = y2 >> 8;
        data[3] = y2 & 0xff;
        write_data(data, 4);

        prev_y1_ = y1;
        prev_y2_ = y2;
    }

    write_command(MIPI_DCS_WRITE_MEMORY_START);
}

void Display::set_address_xy(uint16_t x1, uint16_t y1)
{
    uint8_t command;
    uint8_t data[2];

    x1 = x1 + MIPI_DISPLAY_OFFSET_X;
    y1 = y1 + MIPI_DISPLAY_OFFSET_Y;

    write_command(MIPI_DCS_SET_COLUMN_ADDRESS);
    data[0] = x1 >> 8;
    data[1] = x1 & 0xff;
    write_data(data, 2);

    write_command(MIPI_DCS_SET_PAGE_ADDRESS);
    data[0] = y1 >> 8;
    data[1] = y1 & 0xff;
    write_data(data, 2);

    prev_x1_ = x1;
    prev_y1_ = y1;

    write_command(MIPI_DCS_WRITE_MEMORY_START);
}

void Display::spi_master_init()
{
    hagl_hal_debug("%s\n", "Initialising SPI.");

    gpio_set_function(dc_, GPIO_FUNC_SIO);
    gpio_set_dir(dc_, GPIO_OUT);

    gpio_set_function(cs_, GPIO_FUNC_SIO);
    gpio_set_dir(cs_, GPIO_OUT);

    gpio_set_function(scl_, GPIO_FUNC_SPI);
    gpio_set_function(sda_, GPIO_FUNC_SPI);

    if (MIPI_DISPLAY_PIN_MISO > 0) {
        gpio_set_function(MIPI_DISPLAY_PIN_MISO, GPIO_FUNC_SPI);
    }

    /* Set CS high to ignore any traffic on SPI bus. */
    gpio_put(cs_, 1);

    spi_init(spi_, MIPI_DISPLAY_SPI_CLOCK_SPEED_HZ);
    spi_set_format(spi_, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    uint32_t baud = spi_set_baudrate(spi_, MIPI_DISPLAY_SPI_CLOCK_SPEED_HZ);
    uint32_t peri = clock_get_hz(clk_peri);
    uint32_t sys = clock_get_hz(clk_sys);
    hagl_hal_debug("Baudrate is set to %ld.\n", baud);
    hagl_hal_debug("clk_peri %ld.\n", peri);
    hagl_hal_debug("clk_sys %ld.\n", sys);
}

void Display::init()
{
#ifdef HAGL_HAL_USE_SINGLE_BUFFER
    hagl_hal_debug("%s\n", "Initialising single buffered display.");
#endif /* HAGL_HAL_USE_SINGLE_BUFFER */

#ifdef HAGL_HAL_USE_DOUBLE_BUFFER
    hagl_hal_debug("%s\n", "Initialising double buffered display.");
#endif /* HAGL_HAL_USE_DOUBLE_BUFFER */

#ifdef HAGL_HAL_USE_TRIPLE_BUFFER
    hagl_hal_debug("%s\n", "Initialising triple buffered display.");
#endif /* HAGL_HAL_USE_DOUBLE_BUFFER */

    /* Init the spi driver. */
    //spi_master_init(backend);
    sleep_ms(100);

    /* Reset the display. */
    if (MIPI_DISPLAY_PIN_RST > 0) {
        gpio_set_function(MIPI_DISPLAY_PIN_RST, GPIO_FUNC_SIO);
        gpio_set_dir(MIPI_DISPLAY_PIN_RST, GPIO_OUT);

        gpio_put(MIPI_DISPLAY_PIN_RST, 0);
        sleep_ms(100);
        gpio_put(MIPI_DISPLAY_PIN_RST, 1);
        sleep_ms(100);
    }

    /* Send minimal init commands. */
    write_command(MIPI_DCS_SOFT_RESET);
    sleep_ms(200);

    write_command(MIPI_DCS_SET_ADDRESS_MODE);
    uint8_t data = MIPI_DISPLAY_ADDRESS_MODE;
    write_data(&data, 1);

    data = MIPI_DISPLAY_PIXEL_FORMAT;
    write_command(MIPI_DCS_SET_PIXEL_FORMAT);
    write_data(&data, 1);

#if MIPI_DISPLAY_PIN_TE > 0
    write_command(MIPI_DCS_SET_TEAR_ON);
    data = MIPI_DCS_SET_TEAR_ON_VSYNC;
    write_data(&data, 1);
    hagl_hal_debug("Enable vsync notification on pin %d\n", MIPI_DISPLAY_PIN_TE);
#endif /* MIPI_DISPLAY_PIN_TE > 0 */

#if MIPI_DISPLAY_INVERT
    write_command(MIPI_DCS_ENTER_INVERT_MODE);
    hagl_hal_debug("%s\n", "Inverting display.");
#else
    write_command(MIPI_DCS_EXIT_INVERT_MODE);
#endif /* MIPI_DISPLAY_INVERT */

    write_command(MIPI_DCS_EXIT_SLEEP_MODE);
    sleep_ms(200);

    write_command(MIPI_DCS_SET_DISPLAY_ON);
    sleep_ms(200);

    /* Enable backlight */
    if (MIPI_DISPLAY_PIN_BL > 0) {
        gpio_set_function(MIPI_DISPLAY_PIN_BL, GPIO_FUNC_SIO);
        gpio_set_dir(MIPI_DISPLAY_PIN_BL, GPIO_OUT);
        gpio_put(MIPI_DISPLAY_PIN_BL, 1);
    }

    /* Enable power */
    if (MIPI_DISPLAY_PIN_POWER > 0) {
        gpio_set_function(MIPI_DISPLAY_PIN_POWER, GPIO_FUNC_SIO);
        gpio_set_dir(MIPI_DISPLAY_PIN_POWER, GPIO_OUT);
        gpio_put(MIPI_DISPLAY_PIN_POWER, 1);
    }

    /* Initialise vsync pin */
#if MIPI_DISPLAY_PIN_TE > 0
    gpio_set_function(MIPI_DISPLAY_PIN_TE, GPIO_FUNC_SIO);
    gpio_set_dir(MIPI_DISPLAY_PIN_TE, GPIO_IN);
    gpio_pull_up(MIPI_DISPLAY_PIN_TE);
#endif /* MIPI_DISPLAY_PIN_TE > 0 */

    /* Set the default viewport to full screen. */
    set_address_xyxy(0, 0, MIPI_DISPLAY_WIDTH - 1, MIPI_DISPLAY_HEIGHT - 1);
}

size_t Display::fill_xywh(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, void* _color)
{
    if (0 == w || 0 == h) {
        return 0;
    }

    int32_t x2 = x1 + w - 1;
    int32_t y2 = y1 + h - 1;
    size_t size = w * h;
    uint16_t* color = (uint16_t*)_color;

    set_address_xyxy(x1, y1, x2, y2);

    /* Set DC high to denote incoming data. */
    gpio_put(dc_, 1);

    /* Set CS low to reserve the SPI bus. */
    gpio_put(cs_, 0);

    /* TODO: This assumes 16 bit colors. */
    spi_set_format(spi_, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    while (size--) {
        while (!spi_is_writable(spi_)) {
        };
        spi_get_hw(spi_)->dr = (uint32_t)htons(*color);
    }

    /* Wait for shifting to finish. */
    while (spi_get_hw(spi_)->sr & SPI_SSPSR_BSY_BITS) {
    };
    spi_get_hw(spi_)->icr = SPI_SSPICR_RORIC_BITS;

    spi_set_format(spi_, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    /* Set CS high to ignore any traffic on SPI bus. */
    gpio_put(cs_, 1);

    return size;
}

size_t Display::write_xywh(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint8_t* buffer)
{
    if (0 == w || 0 == h) {
        return 0;
    }

    int32_t x2 = x1 + w - 1;
    int32_t y2 = y1 + h - 1;
    uint32_t size = w * h;

#ifdef HAGL_HAL_USE_SINGLE_BUFFER
    set_address_xyxy(x1, y1, x2, y2);
    write_data(buffer, size * MIPI_DISPLAY_DEPTH / 8);
#endif /* HAGL_HAL_SINGLE_BUFFER */

#ifdef HAGL_HAS_HAL_BACK_BUFFER
    set_address_xyxy(x1, y1, x2, y2);
    write_data(buffer, size * MIPI_DISPLAY_DEPTH / 8);
#endif /* HAGL_HAS_HAL_BACK_BUFFER */
    /* This should also include the bytes for writing the commands. */
    return size * MIPI_DISPLAY_DEPTH / 8;
}

size_t Display::write_xy(uint16_t x1, uint16_t y1, uint8_t* buffer)
{
    set_address_xy(x1, y1);
    write_data(buffer, MIPI_DISPLAY_DEPTH / 8);

    /* This should also include the bytes for writing the commands. */
    return MIPI_DISPLAY_DEPTH / 8;
}

/* TODO: This most likely does not work with dma atm. */
void Display::ioctl(const uint8_t command, uint8_t* data, size_t size)
{
    switch (command) {
        case MIPI_DCS_GET_COMPRESSION_MODE:
        case MIPI_DCS_GET_DISPLAY_ID:
        case MIPI_DCS_GET_RED_CHANNEL:
        case MIPI_DCS_GET_GREEN_CHANNEL:
        case MIPI_DCS_GET_BLUE_CHANNEL:
        case MIPI_DCS_GET_DISPLAY_STATUS:
        case MIPI_DCS_GET_POWER_MODE:
        case MIPI_DCS_GET_ADDRESS_MODE:
        case MIPI_DCS_GET_PIXEL_FORMAT:
        case MIPI_DCS_GET_DISPLAY_MODE:
        case MIPI_DCS_GET_SIGNAL_MODE:
        case MIPI_DCS_GET_DIAGNOSTIC_RESULT:
        case MIPI_DCS_GET_SCANLINE:
        case MIPI_DCS_GET_DISPLAY_BRIGHTNESS:
        case MIPI_DCS_GET_CONTROL_DISPLAY:
        case MIPI_DCS_GET_POWER_SAVE:
        case MIPI_DCS_READ_DDB_START:
        case MIPI_DCS_READ_DDB_CONTINUE:
            write_command(command);
            read_data(data, size);
            break;
        default:
            write_command(command);
            write_data(data, size);
    }
}

void Display::close()
{
    spi_deinit(spi_);
}

void Display::Disable()
{
    enabled_ = false;
}

void Display::Enable()
{
    enabled_ = true;
}

bool Display::Enabled() const
{
    return enabled_;
}

void Display::put_pixel(int16_t x0, int16_t y0, hagl_color_t color)
{
    if (!enabled_) {
        return;
    }
    write_xy(x0, y0, (uint8_t*)&color);
}

void Display::drawHlineInner(int16_t x0, int16_t y0, uint16_t width, hagl_color_t color)
{
    if (!enabled_) {
        return;
    }
    fill_xywh(x0, y0, width, 1, &color);
}

void Display::drawVlineInner(int16_t x0, int16_t y0, uint16_t height, hagl_color_t color)
{
    if (!enabled_) {
        return;
    }
    fill_xywh(x0, y0, 1, height, &color);
}

void Display::blit(int16_t x0, int16_t y0, hagl_bitmap_t* src)
{
    if (!enabled_) {
        return;
    }
    write_xywh(x0, y0, src->width, src->height, (uint8_t*)src->buffer);
}

uint8_t Display::putChar(wchar_t code, int16_t x0, int16_t y0, const unsigned char* font,
    hagl_color_t color, hagl_color_t bgColor)
{
    if (!enabled_) {
        return 0;
    }
    return hagl_put_char(*this, code, x0, y0, color, font, bgColor);
}

uint16_t Display::text(const wchar_t* str, int16_t x0, int16_t y0, const unsigned char* font,
    hagl_color_t color, hagl_color_t bgColor)
{
    if (!enabled_) {
        return 0;
    }
    return hagl_put_text(*this, str, x0, y0, color, font, bgColor);
}

void Display::circle(int16_t x0, int16_t y0, int16_t r, hagl_color_t color, bool fill)
{
    if (!enabled_) {
        return;
    }
    if (fill) {
        hagl_fill_circle(*this, x0, y0, r, color);
    } else {
        hagl_draw_circle(*this, x0, y0, r, color);
    }
}

void Display::ellipse(int16_t x0, int16_t y0, int16_t a, int16_t b, hagl_color_t color, bool fill)
{
    if (!enabled_) {
        return;
    }
    if (fill) {
        hagl_fill_ellipse(*this, x0, y0, a, b, color);
    } else {
        hagl_draw_ellipse(*this, x0, y0, a, b, color);
    }
}

void Display::hline(int16_t x0, int16_t y0, uint16_t width, hagl_color_t color)
{
    if (!enabled_) {
        return;
    }
    hagl_draw_hline_xyw(*this, x0, y0, width, color);
}

void Display::line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, hagl_color_t color)
{
    if (!enabled_) {
        return;
    }
    hagl_draw_line(*this, x0, y0, x1, y1, color);
}

void Display::pixel(int16_t x0, int16_t y0, hagl_color_t color)
{
    if (!enabled_) {
        return;
    }
    hagl_put_pixel(*this, x0, y0, color);
}

void Display::polygon(int16_t amount, int16_t* vertices, hagl_color_t color, bool fill)
{
    if (!enabled_) {
        return;
    }
    if (fill) {
        hagl_fill_polygon(*this, amount, vertices, color);
    } else {
        hagl_draw_polygon(*this, amount, vertices, color);
    }
}

void Display::rectangle(
    int16_t x0, int16_t y0, int16_t x1, int16_t y1, hagl_color_t color, bool fill)
{
    if (!enabled_) {
        return;
    }
    if (fill) {
        hagl_fill_rectangle_xyxy(*this, x0, y0, x1, y1, color);
    } else {
        hagl_draw_rectangle_xyxy(*this, x0, y0, x1, y1, color);
    }
}

void Display::rounded_rectangle(
    int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t r, hagl_color_t color, bool fill)
{
    if (!enabled_) {
        return;
    }
    if (fill) {
        hagl_fill_rounded_rectangle_xyxy(*this, x0, y0, x1, y1, r, color);
    } else {
        hagl_draw_rounded_rectangle_xyxy(*this, x0, y0, x1, y1, r, color);
    }
}

void Display::triangle(Display& display, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2,
    int16_t y2, hagl_color_t color, bool fill)
{
    if (!enabled_) {
        return;
    }
    if (fill) {
        hagl_fill_triangle(display, x0, y0, x1, y1, x2, y2, color);
    } else {
        hagl_draw_triangle(display, x0, y0, x1, y1, x2, y2, color);
    }
}

void Display::vline(int16_t x0, int16_t y0, uint16_t height, hagl_color_t color)
{
    if (!enabled_) {
        return;
    }
    hagl_draw_vline_xyh(*this, x0, y0, height, color);
}
