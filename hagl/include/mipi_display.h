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

#ifndef _MIPI_DISPLAY_H
#define _MIPI_DISPLAY_H


#include <stdint.h>
#include <stddef.h>

#include "hagl_hal.h"

void mipi_display_init(hagl_backend_t *backend);
size_t mipi_display_write_xywh(hagl_backend_t *backend,uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint8_t *buffer);
size_t mipi_display_write_xy(hagl_backend_t *backend,uint16_t x1, uint16_t y1, uint8_t *buffer);
size_t mipi_display_fill_xywh(hagl_backend_t *backend,uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, void *color);
void mipi_display_ioctl(hagl_backend_t *backend,uint8_t command, uint8_t *data, size_t size);
void mipi_display_close(hagl_backend_t *backend);

#endif /* _MIPI_DISPLAY_H */