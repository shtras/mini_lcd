#include "Utils.h"

namespace mini_lcd
{
uint64_t Utils::micros()
{
    return time_us_64();
}

uint64_t Utils::millis()
{
    return micros() / 1000ULL;
}
} // namespace mini_lcd
