#include "Menu.h"
#include "fonts.h"
namespace mini_lcd
{
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

void Menu::SetOnSelect(std::function<void(int)> onSelect)
{
    onSelect_ = onSelect;
}

void Menu::Up()
{
    if (!items_ || items_->empty()) {
        return;
    }
    --selectedIndex_;
    if (selectedIndex_ < 0) {
        selectedIndex_ = static_cast<int>(items_->size()) - 1;
    }
    draw();
}

void Menu::Down()
{
    if (!items_ || items_->empty()) {
        return;
    }
    selectedIndex_ = (selectedIndex_ + 1) % items_->size();
    draw();
}

void Menu::Click()
{
    if (onSelect_) {
        onSelect_(selectedIndex_);
    }
}

void Menu::draw()
{
    if (!display_ || !items_) {
        return;
    }

    display_->clear();
    for (int i = 0; i < static_cast<int>(items_->size()); ++i) {
        auto color = (i == selectedIndex_) ? Color::WHITE : Color::GRAY;
        auto bgColor = (i == selectedIndex_) ? Color::GRAY : Color::BLACK;
        display_->text((*items_)[i].c_str(), 10, (i + 1) * 10, Fonts::font6x9, color, bgColor);
    }
}
} // namespace mini_lcd
