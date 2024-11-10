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

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>

#include "rgb332.h"
#include "rgb565.h"
#include "fontx.h"
#include "hagl/bitmap.h"
#include "hagl/clip.h"
#include "hagl/window.h"

#include "hagl.h"
#include "hagl_hal.h"
#include "Display.h"

void hagl_clear(Display& display)
{
    uint16_t x0 = display.clip.x0;
    uint16_t y0 = display.clip.y0;
    uint16_t x1 = display.clip.x1;
    uint16_t y1 = display.clip.y1;

    hagl_set_clip(display, 0, 0, display.width - 1, display.height - 1);
    hagl_fill_rectangle(display, 0, 0, display.width - 1, display.height - 1, 0x00);
    hagl_set_clip(display, x0, y0, x1, y1);
}

void hagl_init(
    hagl_backend_t* backend, uint8_t scl, uint8_t sda, uint8_t dc, uint8_t cs, spi_inst_t* spi)
{
    memset(backend, 0, sizeof(hagl_backend_t));

    backend->scl = scl;
    backend->sda = sda;
    backend->dc = dc;
    backend->cs = cs;
    backend->spi = spi;

    hagl_hal_init(backend);
    //hagl_set_clip(backend, 0, 0, backend->width - 1, backend->height - 1);
};

size_t hagl_flush(hagl_backend_t* backend)
{
    if (backend->flush) {
        return backend->flush(backend);
    }
    return 0;
};

void hagl_close(hagl_backend_t* backend)
{
    if (backend->close) {
        backend->close(backend);
    }
};
