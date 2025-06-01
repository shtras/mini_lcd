#include "Display.h"
#include "Encoder.h"
#include "Button.h"
#include "Snake.h"
#include "TCP.h"
#include "Comm.h"
#include "PerfGraph.h"
#include "Logger.h"
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

using mini_lcd::Logger;

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

void colorTest(Display& display)
{
    hagl_clear(display);
    constexpr auto width = 20;
    constexpr auto height = 20;

    constexpr auto numX = display.width / width;
    constexpr auto numY = display.height / height;

    for (int y = 0; y < numY; ++y) {
        for (int x = 0; x < numX; ++x) {
            auto colorIdx = (y * numX + x) % Color::colors.size();
            display.rectangle(x * width, y * height, (x + 1) * width - 1, (y + 1) * height - 1,
                Color::colors[colorIdx], true);
            display.text(std::to_wstring(colorIdx).c_str(), x * width + 1, y * height + 1,
                Color::WHITE, Fonts::font5x7);
        }
    }
}

void displayThread()
{
    mini_lcd::Receiver receiver;
    gpio_pull_up(15); // SPI on pin 15

    Display display1(10, 11, 3, 2, spi1);
    Display miscDisplay(10, 11, 0, 1, spi1);
    Display cpuDisplay(10, 11, 6, 7, spi1);
    Display display2(10, 11, 17, 16, spi1);

    mini_lcd::PerfGraph perfGraph(&cpuDisplay, &miscDisplay);

    mipi_display_spi_master_init();

    cpuDisplay.init();
    miscDisplay.init();
    display1.init();
    display2.init();

    int32_t iteration = 0;

    hagl_clear(cpuDisplay);
    hagl_clear(miscDisplay);
    hagl_clear(display1);
    hagl_clear(display2);

    display2.rectangle(0, 0, display2.width, display2.height, Color::DARK_BROWN, true);

    colorTest(display1);

    std::wstring str1 = L"Something good";
    std::wstring str2 = L"coming to this";
    std::wstring str3 = L" display soon";

    auto startTime = millis();

    display2.text(str1.c_str(), 20, 20, Color::DARK_GREEN, Fonts::font6x9);
    display2.text(str2.c_str(), 20, 35, Color::DARK_GREEN, Fonts::font6x9);
    display2.text(str3.c_str(), 20, 50, Color::DARK_GREEN, Fonts::font6x9);

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
        display2.text(
            ss.str().c_str(), 10, cpuDisplay.height / 1.5, Color::DARK_YELLOW, Fonts::font5x7);

        ++iteration;
    };
}

void mainThread()
{
    mini_lcd::Encoder encoder(20, 21, 22, []{
        Logger::info() << "Encoder: Button pressed\n";
    });
    mini_lcd::Encoder encoder1(4, 5, 28, []{
        Logger::info() << "Encoder1: Button pressed\n";
    });
    encoder.setOnLeft([]() { Logger::info() << "Encoder: Left\n"; });
    encoder.setOnRight([]() { Logger::info() << "Encoder: Right\n"; });
    encoder1.setOnLeft([]() { Logger::info() << "Encoder1: Left\n"; });
    encoder1.setOnRight([]() { Logger::info() << "Encoder1: Right\n"; });

    // encoder.setOnLeft([&snake]() { snake.left(); });
    // encoder.setOnRight([&snake]() { snake.right(); });

    mini_lcd::Button button(18, []{
        Logger::info() << "Button 18 up\n";
    }, []{
        Logger::info() << "Button 18 down\n";
    });
    mini_lcd::Button button1(19, []{
        Logger::info() << "Button 19 up\n";
    }, []{
        Logger::info() << "Button 19 down\n";
    });

    Timestamp lastTime = millis();
    mini_lcd::TCPTest tcp;
    while (!tcp.connect())
        ;

    while (1) {
        Timestamp currentTime = millis();
        if (currentTime - lastTime > 5000) {
            lastTime = currentTime;
            tcp.getMeasurements();
        }
        button.Process();
        button1.Process();
        encoder.process();
        encoder1.process();
    }
}

int main()
{
    stdio_init_all();
    sleep_ms(2000);
    Logger::info() << "Start!\n";
    multicore_launch_core1(displayThread);
    mainThread();
}
