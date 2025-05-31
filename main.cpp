#include "Display.h"
#include "Encoder.h"
#include "Snake.h"
#include "TCP.h"
#include "Comm.h"
#include "PerfGraph.h"
#include "ino_compat.h"

#include <hagl_hal.h>
#include <hagl.h>
#include <fonts.h>

#include <pico/stdlib.h>
#include <pico/binary_info.h>
#include <pico/multicore.h>
#include <hardware/spi.h>

#include <iostream>
#include <iomanip>
#include <array>
#include <sstream>

void foo1(Display& display)
{
    hagl_clear(display);
    int x = 5;
    int y = 5;
    for (;;) {
        hagl_put_pixel(display, x, y, hagl_color(255, 0, 0));
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
    spi_init(spi0, MIPI_DISPLAY_SPI_CLOCK_SPEED_HZ);
    spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    spi_init(spi1, MIPI_DISPLAY_SPI_CLOCK_SPEED_HZ);
    spi_set_format(spi1, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
}

void dots(Display& display)
{
    hagl_draw_circle(display, rand() % display.width, rand() % display.height, 2,
        hagl_color(rand() % 255, rand() % 255, rand() % 255));
}

void squares(Display& display)
{
    static int x = 0;
    static int y = 0;
    static int dx = 5;
    static int dy = 3;

    hagl_fill_rounded_rectangle(display, x, y, x + 10, y + 10, 5, hagl_color(0, 0, 0));

    x += dx;
    y += dy;

    if (x >= display.width - 10 || x <= 0) {
        dx = -dx;
    }
    if (y >= display.height - 10 || y <= 0) {
        dy = -dy;
    }

    hagl_fill_rounded_rectangle(display, x, y, x + 10, y + 10, 5, hagl_color(0, 255, 255));
}

void circles(Display& display)
{
    static int r = 2;
    static bool expanding = true;
    hagl_draw_circle(display, display.width / 2, display.height / 2, r, hagl_color(0, 0, 0));
    if (expanding) {
        r++;
        if (r > display.width / 2) {
            expanding = false;
        }
    } else {
        r--;
        if (r < 2) {
            expanding = true;
        }
    }
    hagl_draw_circle(display, display.width / 2, display.height / 2, r, hagl_color(255, 255, 0));
}

void texts(Display& display)
{
    std::array<std::wstring, 15> strs = {
        L"MOV AX, 0x0A      ; Load value 0x0A into AX register",
        L"ADD AX, 0x05      ; Add value 0x05 to AX",
        L"CMP AX, 0x0F      ; Compare AX with 0x0F",
        L"JLE SHORT label1  ; Jump to label1 if AX is less than or equal to 0x0F",
        L"MOV BX, AX        ; Move value from AX to BX",
        L"SHL BX, 1         ; Shift BX left by 1 bit",
        L"SUB BX, 0x03      ; Subtract 0x03 from BX",
        L"label1:",
        L"MOV CX, BX        ; Copy BX to CX",
        L"XOR CX, 0xFF      ; Perform XOR with value 0xFF",
        L"INC CX            ; Increment CX by 1",
        L"JMP SHORT label2  ; Unconditional jump to label2",
        L"label2:",
        L"NOP               ; No operation (do nothing)",
        L"HLT               ; Halt the processor",
    };

    static int start_idx = 0;
    static Timestamp last_time = millis();
    if (millis() - last_time > 300) {
        last_time = millis();
        start_idx = (start_idx + 1) % strs.size();
        hagl_clear(display);

        for (int i = 0; i < static_cast<int>(strs.size()); ++i) {
            int idx = (start_idx + i) % strs.size();
            hagl_put_text(
                display, strs[idx].c_str(), 1, 1 + i * 10, hagl_color(255, 100, 0), Fonts::font6x9);
        }
    }
}

bool checkingDirection = false;

void irq_callback(uint gpio, uint32_t event)
{
    std::cout << "GPIO " << gpio << " event " << event << " ";
    if (event & GPIO_IRQ_EDGE_RISE) {
        std::cout << "Rise ";
    }
    if (event & GPIO_IRQ_EDGE_FALL) {
        std::cout << "Fall ";
    }
    if (event & GPIO_IRQ_LEVEL_HIGH) {
        std::cout << "High ";
    }
    if (event & GPIO_IRQ_LEVEL_LOW) {
        std::cout << "Low ";
    }
    std::cout << "\n";
    if (event == GPIO_IRQ_EDGE_RISE) {
        checkingDirection = false;
    }
    if (event == GPIO_IRQ_EDGE_FALL) {
        if (checkingDirection) {
            if (gpio == 15) {
                std::cout << "Right\n";
            } else {
                std::cout << "Left\n";
            }
        }
        checkingDirection = true;
    }
}

void displayThread()
{
    mini_lcd::Receiver receiver;
    gpio_pull_up(15); // SPI on pin 15

    Display cpuDisplay(10, 11, 3, 2, spi1);
    Display miscDisplay(10, 11, 0, 1, spi1);
    Display display1(10, 11, 6, 7, spi1);
    Display display2(10, 11, 17, 16, spi1);

    mini_lcd::PerfGraph perfGraph(&cpuDisplay, &miscDisplay);

    mipi_display_spi_master_init();

    cpuDisplay.init();
    miscDisplay.init();
    display1.init();
    display2.init();

    std::array<uint16_t, 4> colors = {hagl_color(255, 0, 0), hagl_color(0, 255, 0),
        hagl_color(0, 0, 255), hagl_color(255, 255, 255)};

    int32_t iteration = 0;

    hagl_clear(cpuDisplay);
    hagl_clear(miscDisplay);
    hagl_clear(display1);
    hagl_clear(display2);

    std::wstring str1 = L"Something good";
    std::wstring str2 = L"coming to this";
    std::wstring str3 = L" display soon";

    auto startTime = millis();

    hagl_put_text(display1, str1.c_str(), 20, 20, hagl_color(255, 100, 0), Fonts::font6x9);
    hagl_put_text(display1, str2.c_str(), 20, 35, hagl_color(255, 100, 0), Fonts::font6x9);
    hagl_put_text(display1, str3.c_str(), 20, 50, hagl_color(255, 100, 0), Fonts::font6x9);
    hagl_put_text(display2, str1.c_str(), 20, 20, hagl_color(255, 100, 0), Fonts::font6x9);
    hagl_put_text(display2, str2.c_str(), 20, 35, hagl_color(255, 100, 0), Fonts::font6x9);
    hagl_put_text(display2, str3.c_str(), 20, 50, hagl_color(255, 100, 0), Fonts::font6x9);

    while (1) {
        auto msg = receiver.process();
        if (msg && msg->type == mini_lcd::Message::Type::Measurements) {
            perfGraph.AddData(*msg);
            perfGraph.Draw();
        }
        //sleep_ms(100);
        auto currTime = millis();

        std::wstringstream ss;
        ss << "Running for: " << std::fixed << std::setprecision(2)
           << (currTime - startTime) / 1000.0f << " s";
        hagl_put_text(
            display2, ss.str().c_str(), 10, cpuDisplay.height / 1.5, colors[3], Fonts::font5x7);

        ++iteration;
    };
}

void mainThread()
{
    // mini_lcd::Encoder encoder(14, 15, 18);
    // mini_lcd::Encoder encoder1(12, 13, 18);
    // encoder.setOnLeft([]() { std::cout << "Encoder: Left\n"; });
    // encoder.setOnRight([]() { std::cout << "Encoder: Right\n"; });
    // encoder1.setOnLeft([]() { std::cout << "Encoder1: Left\n"; });
    // encoder1.setOnRight([]() { std::cout << "Encoder1: Right\n"; });

    // encoder.setOnLeft([&snake]() { snake.left(); });
    // encoder.setOnRight([&snake]() { snake.right(); });

    Timestamp lastTime = millis();
    mini_lcd::TCPTest tcp;
    while (!tcp.connect())
        ;

    while (1) {
        Timestamp currentTime = millis();
        if (currentTime - lastTime > 5000) {
            lastTime = currentTime;
            std::cout << "Main thread running...\n";
            tcp.getMeasurements();
        }
        // encoder.process();
        // encoder1.process();
    }
}

int main()
{
    stdio_init_all();
    sleep_ms(2000);
    std::cout << "Start!\n";
    multicore_launch_core1(displayThread);
    mainThread();
}
