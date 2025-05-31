#pragma once

#include <functional>

namespace mini_lcd
{
class Button
{
public:
    explicit Button(int pin, std::function<void(void)> onDown = nullptr,
        std::function<void(void)> onUp = nullptr);

    void Process();

private:
    int pin_ = 0;
    std::function<void(void)> onDown_ = nullptr;
    std::function<void(void)> onUp_ = nullptr;
    int status_ = 1;
};
} // namespace mini_lcd
