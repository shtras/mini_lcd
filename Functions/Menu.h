#pragma once
#include "Display.h"

#include <functional>
#include <vector>
#include <string>

namespace mini_lcd
{
class Menu
{
public:
    explicit Menu(Display* display, std::function<int(void)> onSelect = nullptr);
    void SetDisplay(Display* display);
    void SetItems(std::vector<std::wstring>* items);

    void Up();
    void Down();
    void Click();

private:
    void draw();

    Display* display_ = nullptr;
    std::function<int(void)> onSelect_ = nullptr;
    std::vector<std::wstring>* items_ = nullptr;
    int selectedIndex_ = -1;
};
} // namespace mini_lcd
