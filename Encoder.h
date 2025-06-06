#pragma once

#include "Button.h"

#include "pico/stdlib.h"

#include <list>
#include <memory>
#include <functional>

namespace mini_lcd
{
class Encoder
{
public:
    Encoder(uint pinA, uint pinB, uint pinButton, std::function<void(void)> onPress = nullptr);
    void process();
    void setOnLeft(std::function<void()> onLeft) { onLeft_ = onLeft; }
    void setOnRight(std::function<void()> onRight) { onRight_ = onRight; }

private:
    enum class Direction { Left, Right };
    static void irq_callback(uint gpio, uint32_t event);
    void callback(uint gpio, uint32_t event);

    uint pinA_ = -1;
    uint pinB_ = -1;
    
    std::shared_ptr<Button> button_ = nullptr;

    bool checkingDirection_ = false;
    std::list<Direction> directions_;

    std::function<void()> onLeft_ = nullptr;
    std::function<void()> onRight_ = nullptr;
};
} // namespace mini_lcd
