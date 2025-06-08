#include "Menu.h"
#include "fonts.h"
namespace mini_lcd
{
Menu::Menu(Display* display, std::function<int(void)> onSelect)
    : display_(display)
    , onSelect_(onSelect)
{
}

void Menu::SetItems(std::vector<std::wstring>* items)
{
    if (!items || items->empty()) {
        items_ = nullptr;
        selectedIndex_ = -1;
        return;
    }
    items_ = items;
    selectedIndex_ = 0;
    draw();
}

void Menu::SetDisplay(Display* display)
{
    if (display_) {
        display_->clear();
    }
    display_ = display;
}

void Menu::Up() {
    if (!items_ || items_->empty()) {
        return;
    }
    selectedIndex_ = (selectedIndex_ - 1) % items_->size();
    draw();
}

void Menu::Down() {
    if (!items_ || items_->empty()) {
        return;
    }
    selectedIndex_ = (selectedIndex_ + 1) % items_->size();
    draw();
}

void Menu::draw()
{
    if (!display_ || !items_) {
        return;
    }

    display_->clear();
    for (int i = 0; i < static_cast<int>(items_->size()); ++i) {
        auto color = (i == selectedIndex_) ? Color::WHITE : Color::GRAY;
        auto bgColor = (i == selectedIndex_) ? Color::DARK_GRAY : Color::BLACK;
        display_->text((*items_)[i].c_str(), 0, static_cast<int>(i * 10), Fonts::font5x7, color, bgColor);
    }
}
} // namespace mini_lcd
