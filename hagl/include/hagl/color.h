
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

#include <array>

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

constexpr hagl_color_t hagl_color(uint32_t rgb)
{
    return rgb565((rgb & 0xff0000) >> 16, (rgb & 0x00ff00) >> 8, rgb & 0x0000ff);
}

struct Color
{
    static constexpr hagl_color_t BLACK = hagl_color(0x00, 0x00, 0x00);      // 0
    static constexpr hagl_color_t WHITE = hagl_color(0xFF, 0xFF, 0xFF);      // 1
    static constexpr hagl_color_t RED = hagl_color(0xFF, 0x00, 0x00);        // 2
    static constexpr hagl_color_t GREEN = hagl_color(0x00, 0xFF, 0x00);      // 3
    static constexpr hagl_color_t BLUE = hagl_color(0x00, 0x00, 0xFF);       // 4
    static constexpr hagl_color_t YELLOW = hagl_color(0xFF, 0xFF, 0x00);     // 5
    static constexpr hagl_color_t CYAN = hagl_color(0x00, 0xFF, 0xFF);       // 6
    static constexpr hagl_color_t MAGENTA = hagl_color(0xFF, 0x00, 0xFF);    // 7
    static constexpr hagl_color_t GRAY = hagl_color(0x80, 0x80, 0x80);       // 8
    static constexpr hagl_color_t LIGHT_GRAY = hagl_color(0xC0, 0xC0, 0xC0); // 9
    static constexpr hagl_color_t DARK_GRAY = hagl_color(0x40, 0x40, 0x40);  // 10
    static constexpr hagl_color_t ORANGE = hagl_color(0xFF, 0xA5, 0x00);     // 11
    static constexpr hagl_color_t PURPLE = hagl_color(0x80, 0x00, 0x80);     // 12
    static constexpr hagl_color_t PINK = hagl_color(0xFF, 0xC0, 0xCB);       // 13
    static constexpr hagl_color_t BROWN = hagl_color(0xA5, 0x2A, 0x2A);      // 14

    static constexpr hagl_color_t LIGHT_GREEN = hagl_color(0x90, 0xEE, 0x90);   // 15
    static constexpr hagl_color_t LIGHT_RED = hagl_color(0xFF, 0x47, 0x4c);     // 16
    static constexpr hagl_color_t LIGHT_YELLOW = hagl_color(0xFF, 0xFF, 0xE0);  // 17
    static constexpr hagl_color_t LIGHT_CYAN = hagl_color(0xE0, 0xFF, 0xFF);    // 18
    static constexpr hagl_color_t LIGHT_MAGENTA = hagl_color(0xFF, 0xE0, 0xFF); // 19
    static constexpr hagl_color_t LIGHT_ORANGE = hagl_color(0xFF, 0xD7, 0x00);  // 20
    static constexpr hagl_color_t LIGHT_PINK = hagl_color(0xFF, 0xB6, 0xC1);    // 21
    static constexpr hagl_color_t LIGHT_BROWN = hagl_color(0xCD, 0x85, 0x3F);   // 22

    static constexpr hagl_color_t DARK_GREEN = hagl_color(0x00, 0x64, 0x00);   // 23
    static constexpr hagl_color_t DARK_RED = hagl_color(0x8B, 0x00, 0x00);     // 24
    static constexpr hagl_color_t DARK_BLUE = hagl_color(0x00, 0x00, 0x8B);    // 25
    static constexpr hagl_color_t DARK_YELLOW = hagl_color(0x8B, 0x80, 0x00);  // 26
    static constexpr hagl_color_t DARK_CYAN = hagl_color(0x00, 0x8B, 0x8B);    // 27
    static constexpr hagl_color_t DARK_MAGENTA = hagl_color(0x8B, 0x00, 0x8B); // 28
    static constexpr hagl_color_t DARK_ORANGE = hagl_color(0xFF, 0x8C, 0x00);  // 29
    static constexpr hagl_color_t DARK_PURPLE = hagl_color(0x48, 0x00, 0x48);  // 30
    static constexpr hagl_color_t DARK_PINK = hagl_color(0xAA336A);            // 31
    static constexpr hagl_color_t DARK_BROWN = hagl_color(0x362204);           // 32

    static constexpr std::array<hagl_color_t, 33> colors = {BLACK, WHITE, RED, GREEN, BLUE, YELLOW,
        CYAN, MAGENTA, GRAY, LIGHT_GRAY, DARK_GRAY, ORANGE, PURPLE, PINK, BROWN, LIGHT_GREEN,
        LIGHT_RED, LIGHT_YELLOW, LIGHT_CYAN, LIGHT_MAGENTA, LIGHT_ORANGE, LIGHT_PINK, LIGHT_BROWN,
        DARK_GREEN, DARK_RED, DARK_BLUE, DARK_YELLOW, DARK_CYAN, DARK_MAGENTA, DARK_ORANGE,
        DARK_PURPLE, DARK_PINK, DARK_BROWN};
};

#endif /* _HAGL_COLOR_H */