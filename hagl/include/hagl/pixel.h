
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

#ifndef _HAGL_PIXEL_H
#define _HAGL_PIXEL_H

#include <stdint.h>

#include "hagl/color.h"
#include "Display.h"


/**
 * Put a single pixel
 *
 * Output will be clipped to the current clip window.
 *
 * @param display
 * @param x0
 * @param y0
 * @param color
 */
void hagl_put_pixel(Display& display, int16_t x0, int16_t y0, hagl_color_t color);

/**
 * Get a single pixel
 *
 * Input will be clipped to the current clip window. In case of
 * error or if HAL does not support this feature returns black.
 *
 * @param display
 * @param x0
 * @param y0
 * @return color at the given location
 */
hagl_color_t hagl_get_pixel(Display& display, int16_t x0, int16_t y0);


#endif /* _HAGL_PIXEL_H */