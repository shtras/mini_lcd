
/*

MIT License

Copyright (c) 2018-2023 Mika Tuupola

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

This file is part of the HAGL graphics library:
https://github.com/tuupola/hagl


SPDX-License-Identifier: MIT

*/

#ifndef _HAGL_COLOR_H
#define _HAGL_COLOR_H

#include <hagl_hal_color.h>
#include "rgb565.h"

class Display;
/**
 * Convert RGB to color
 *
 * Returns color type  defined by the HAL. Most often it is an
 * uint16_t RGB565 color.
 *
 * @param display
 * @return color
 */

constexpr hagl_color_t hagl_color(uint8_t r, uint8_t g, uint8_t b)
{
    return rgb565(r, g, b);
}


struct Color {
static constexpr hagl_color_t BLACK = hagl_color(0x00, 0x00, 0x00);
static constexpr hagl_color_t WHITE = hagl_color(0xFF, 0xFF, 0xFF);
static constexpr hagl_color_t RED = hagl_color(0xFF, 0x00, 0x00);
static constexpr hagl_color_t GREEN = hagl_color(0x00, 0xFF, 0x00);
static constexpr hagl_color_t BLUE = hagl_color(0x00, 0x00, 0xFF);
static constexpr hagl_color_t YELLOW = hagl_color(0xFF, 0xFF, 0x00);
static constexpr hagl_color_t CYAN = hagl_color(0x00, 0xFF, 0xFF);
static constexpr hagl_color_t MAGENTA = hagl_color(0xFF, 0x00, 0xFF);
static constexpr hagl_color_t GRAY = hagl_color(0x80, 0x80, 0x80);
static constexpr hagl_color_t LIGHT_GRAY = hagl_color(0xC0, 0xC0, 0xC0);
static constexpr hagl_color_t DARK_GRAY = hagl_color(0x40, 0x40, 0x40);
static constexpr hagl_color_t ORANGE = hagl_color(0xFF, 0xA5, 0x00);
static constexpr hagl_color_t PURPLE = hagl_color(0x80, 0x00, 0x80);
static constexpr hagl_color_t PINK = hagl_color(0xFF, 0xC0, 0xCB);
static constexpr hagl_color_t BROWN = hagl_color(0xA5, 0x2A, 0x2A);
static constexpr hagl_color_t LIGHT_GREEN = hagl_color(0x90, 0xEE, 0x90);
static constexpr hagl_color_t LIGHT_RED = hagl_color(0xFF, 0xB6, 0xC1);
static constexpr hagl_color_t LIGHT_YELLOW = hagl_color(0xFF, 0xFF, 0xE0);
static constexpr hagl_color_t LIGHT_CYAN = hagl_color(0xE0, 0xFF, 0xFF);
static constexpr hagl_color_t LIGHT_MAGENTA = hagl_color(0xFF, 0xE0, 0xFF);
static constexpr hagl_color_t LIGHT_ORANGE = hagl_color(0xFF, 0xD7, 0x00);
static constexpr hagl_color_t LIGHT_PINK = hagl_color(0xFF, 0xB6, 0xC1);
static constexpr hagl_color_t LIGHT_BROWN = hagl_color(0xCD, 0x85, 0x3F);
static constexpr hagl_color_t LIGHT_GRAYISH_BLUE = hagl_color(0xB0, 0xC4, 0xDE);
static constexpr hagl_color_t LIGHT_GRAYISH_GREEN = hagl_color(0x90, 0xEE, 0x90);
static constexpr hagl_color_t LIGHT_GRAYISH_RED = hagl_color(0xFF, 0xA0, 0x7A);
static constexpr hagl_color_t LIGHT_GRAYISH_YELLOW = hagl_color(0xFA, 0xF0, 0xE6);
static constexpr hagl_color_t LIGHT_GRAYISH_CYAN = hagl_color(0xE0, 0xFF, 0xFF);
static constexpr hagl_color_t LIGHT_GRAYISH_MAGENTA = hagl_color(0xFF, 0xB6, 0xC1);
static constexpr hagl_color_t LIGHT_GRAYISH_ORANGE = hagl_color(0xFF, 0xD7, 0x00);
static constexpr hagl_color_t LIGHT_GRAYISH_PINK = hagl_color(0xFF, 0xB6, 0xC1);
static constexpr hagl_color_t LIGHT_GRAYISH_BROWN = hagl_color(0xCD, 0x85, 0x3F);
};


#endif /* _HAGL_COLOR_H */