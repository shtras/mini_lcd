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

#include <stdint.h>

#include "Display.h"

#include "hagl/color.h"
#include "hagl/backend.h"

void hagl_put_pixel(Display& display, int16_t x0, int16_t y0, hagl_color_t color)
{
    /* x0 or y0 is before the edge, nothing to do. */
    if ((x0 < display.clip.x0) || (y0 < display.clip.y0)) {
        return;
    }

    /* x0 or y0 is after the edge, nothing to do. */
    if ((x0 > display.clip.x1) || (y0 > display.clip.y1)) {
        return;
    }

    /* If still in bounds set the pixel. */
    display.put_pixel(x0, y0, color);
}

hagl_color_t hagl_get_pixel(Display& display, int16_t x0, int16_t y0)
{
    /* x0 or y0 is before the edge, nothing to do. */
    if ((x0 < display.clip.x0) || (y0 < display.clip.y0)) {
        return hagl_color(0, 0, 0);
    }

    /* x0 or y0 is after the edge, nothing to do. */
    if ((x0 > display.clip.x1) || (y0 > display.clip.y1)) {
        return hagl_color(0, 0, 0);
    }

    //if (display.get_pixel) {
    //    return display.get_pixel(display, x0, y0);
    //}

    return hagl_color(0, 0, 0);
}
