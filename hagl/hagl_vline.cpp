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

#include "hagl/line.h"
#include "hagl/backend.h"
#include "hagl/color.h"
#include "Display.h"

void hagl_draw_vline_xyh(Display& display, int16_t x0, int16_t y0, uint16_t h, hagl_color_t color)
{
    int16_t height = h;

    /* x0 or y0 is over the edge, nothing to do. */
    if ((x0 > display.clip.x1) || (x0 < display.clip.x0) || (y0 > display.clip.y1)) {
        return;
    }

    /* y0 is top of clip window, ignore start part. */
    if (y0 < display.clip.y0) {
        height = height + y0;
        y0 = display.clip.y0;
    }

    /* Everything outside clip window, nothing to do. */
    if (height <= 0) {
        return;
    }

    /* Cut anything going over right edge. */
    if (((y0 + height) > display.clip.y1)) {
        height = height - (y0 + height - 1 - display.clip.y1);
    }

    display.drawVlineInner(x0, y0, height, color);
}
