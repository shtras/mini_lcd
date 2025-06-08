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
    void SetDisplay(Display* display);
    void SetItems(std::vector<std::wstring>* items);
    void SetOnSelect(std::function<void(int)> onSelect);

    void Up();
    void Down();
    void Click();

private:
    void draw();

    Display* display_ = nullptr;
    std::function<void(int)> onSelect_ = nullptr;
    std::vector<std::wstring>* items_ = nullptr;
    int selectedIndex_ = -1;
};
} // namespace mini_lcd
