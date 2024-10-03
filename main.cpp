#include <hagl_hal.h>
#include <hagl.h>
#include <font6x9.h>

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include <hardware/spi.h>

#include <iostream>
#include <array>
#include <sstream>

void foo1(hagl_backend_t* display)
{
    hagl_clear(display);
    int x = 5;
    int y = 5;
    for (;;) {
        hagl_put_pixel(display, x, y, hagl_color(display, 255, 0, 0));
        sleep_ms(50);
        ++x;
        y += 2;
        if (x > 10) {
            x = 5;
        }
        if (y > 20) {
            y = 5;
        }
    }
}

static void mipi_display_spi_master_init()
{
    hagl_hal_debug("%s\n", "Initialising SPI.");

    gpio_set_function(6, GPIO_FUNC_SIO);
    gpio_set_dir(6, GPIO_OUT);

    gpio_set_function(7, GPIO_FUNC_SIO);
    gpio_set_dir(7, GPIO_OUT);

    gpio_set_function(14, GPIO_FUNC_SIO);
    gpio_set_dir(14, GPIO_OUT);

    gpio_set_function(13, GPIO_FUNC_SIO);
    gpio_set_dir(13, GPIO_OUT);

    gpio_set_function(0, GPIO_FUNC_SIO);
    gpio_set_dir(0, GPIO_OUT);

    gpio_set_function(1, GPIO_FUNC_SIO);
    gpio_set_dir(1, GPIO_OUT);

    gpio_set_function(10, GPIO_FUNC_SPI);
    gpio_set_function(11, GPIO_FUNC_SPI);

    /* Set CS high to ignore any traffic on SPI bus. */
    gpio_put(7, 1);
    gpio_put(13, 1);
    gpio_put(1, 1);

    spi_init(spi0, MIPI_DISPLAY_SPI_CLOCK_SPEED_HZ);
    spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    spi_init(spi1, MIPI_DISPLAY_SPI_CLOCK_SPEED_HZ);
    spi_set_format(spi1, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
}

void dots(hagl_backend_t* display)
{
    hagl_draw_circle(display, rand() % display->width, rand() % display->height, 2,
        hagl_color(display, rand() % 255, rand() % 255, rand() % 255));
}

void squares(hagl_backend_t* display)
{
    static int x = 0;
    static int y = 0;
    static int dx = 5;
    static int dy = 3;

    hagl_fill_rounded_rectangle(display, x, y, x + 10, y + 10, 5, hagl_color(display, 0, 0, 0));

    x += dx;
    y += dy;

    if (x >= display->width - 10 || x <= 0) {
        dx = -dx;
    }
    if (y >= display->height - 10 || y <= 0) {
        dy = -dy;
    }

    hagl_fill_rounded_rectangle(display, x, y, x + 10, y + 10, 5, hagl_color(display, 0, 255, 255));
}

void circles(hagl_backend_t* display)
{
    static int r = 2;
    static bool expanding = true;
    hagl_draw_circle(
        display, display->width / 2, display->height / 2, r, hagl_color(display, 0, 0, 0));
    if (expanding) {
        r++;
        if (r > display->width / 2) {
            expanding = false;
        }
    } else {
        r--;
        if (r < 2) {
            expanding = true;
        }
    }
    hagl_draw_circle(
        display, display->width / 2, display->height / 2, r, hagl_color(display, 255, 255, 0));
}

int main()
{
    stdio_init_all();
    std::array<hagl_backend_t, 3> displays;

    mipi_display_spi_master_init();

    hagl_init(&displays[0], 10, 11, 14, 13, spi1);
    hagl_init(&displays[1], 10, 11, 6, 7, spi1);
    hagl_init(&displays[2], 10, 11, 0, 1, spi1);

    std::array<uint16_t, 4> colors = {hagl_color(&displays[0], 255, 0, 0),
        hagl_color(&displays[0], 0, 255, 0), hagl_color(&displays[0], 0, 0, 255),
        hagl_color(&displays[0], 255, 255, 255)};

    int32_t iteration = 0;

    hagl_clear(&displays[0]);
    hagl_clear(&displays[1]);
    hagl_clear(&displays[2]);

    while (1) {
        squares(&displays[0]);
        circles(&displays[1]);
        dots(&displays[2]);
        if (iteration % 100 == 0) {
            hagl_clear(&displays[2]);
        }
        sleep_ms(10);
        std::wstringstream ss;
        ss << "Iteration: " << iteration;
        //hagl_put_text(&display, ss.str().c_str(), 10, display.height / 5, colors[3], font6x9);
        //hagl_put_text(&display1, ss.str().c_str(), 20, display.height / 2, colors[3], font6x9);
        //hagl_put_text(&display3, ss.str().c_str(), 20, display.height / 1.5, colors[3], font6x9);

        ++iteration;
    };
}
