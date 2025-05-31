#include "Button.h"

#include <pico/stdlib.h>
#include <pico/binary_info.h>

#include <iostream>

namespace mini_lcd
{
Button::Button(int pin, std::function<void(void)> onDown, std::function<void(void)> onUp)
    : pin_(pin)
    , onDown_(onDown)
    , onUp_(onUp)
{
    gpio_init(pin_);
    gpio_set_dir(pin_, GPIO_IN);
    gpio_pull_up(pin_);
}

void Button::Process()
{
    auto status = gpio_get(pin_);
    if (status != status_) {
        status_ = status;
        if (status == 1) {
            if (onDown_) {
                onDown_();
            }
        } else {
            if (onUp_) {
                onUp_();
            }
        }
    }
}
} // namespace mini_lcd
