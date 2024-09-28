#include <hagl_hal.h>
#include <hagl.h>

#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include <iostream>
#include <array>

int main()
{
    stdio_init_all();

    hagl_backend_t *display = hagl_init();

    std::array<uint16_t, 4> colors = {0xf000, 0x0f00, 0x00f0, 0x000f};

    while (1)
    {
        hagl_clear(display);
        sleep_ms(1000);
        int N = 15;
        uint16_t stepX = display->width / N / 2;
        uint16_t stepY = display->height / N / 2;
        for (uint16_t i = 0; i < N; i++)
        {
            hagl_draw_rectangle(display, i * stepX, i * stepY, display->width - i * stepX - 1, display->height - i * stepY - 1, colors[i % colors.size()]);
            sleep_ms(500);
        }
        hagl_flush(display);
        sleep_ms(1000);
    };

    hagl_close(display);
}
