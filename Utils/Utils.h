#pragma once

#include "pico/stdlib.h"

namespace mini_lcd
{
struct Utils
{
    static uint64_t micros();

    static uint64_t millis();
};
} // namespace mini_lcd