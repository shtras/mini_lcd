#include "Snake.h"

#include <fonts.h>

#include <hagl_hal.h>
#include <hagl.h>

#include <algorithm>

namespace mini_lcd
{
Snake::Snake(Display* display)
    : display_(display)
    , width_(Display::width / 10)
    , height_(Display::height / 10)
{
    reset();
}

void Snake::SetDisplay(Display* display)
{
    if (display_) {
        display_->clear();
    }
    display_ = display;
    reset();
}

void Snake::Process()
{
    if (!display_) {
        return;
    }
    auto now = Utils::millis();
    if (now - last_time_ < speed_) {
        return;
    }
    if (gameOver_) {
        return;
    }
    directionChanged_ = false;
    last_time_ = now;
    Segment nextHead = performStep(segments_.front(), direction_);

    if (nextHead.x < 1 || nextHead.x > width_ - 1 || nextHead.y < 1 || nextHead.y > height_ - 2 ||
        std::any_of(segments_.begin(), segments_.end(), [nextHead](const Segment& segment) {
            return nextHead.x == segment.x && nextHead.y == segment.y;
        })) {
        gameOver_ = true;
        display_->text(L"Game Over", 10, 10, Fonts::font5x7, hagl_color(255, 0, 0));
        return;
    }

    segments_.push_front(nextHead);

    if (nextHead.x == apple_.x && nextHead.y == apple_.y) {
        erase(apple_);
        spawnApple();
    } else {
        erase(segments_.back());
        segments_.pop_back();
    }
    draw(nextHead);
}

void Snake::left()
{
    if (gameOver_) {
        reset();
        gameOver_ = false;
        return;
    }
    if (directionChanged_) {
        return;
    }
    directionChanged_ = true;
    switch (direction_) {
        case Direction::Up:
            direction_ = Direction::Left;
            break;
        case Direction::Left:
            direction_ = Direction::Down;
            break;
        case Direction::Down:
            direction_ = Direction::Right;
            break;
        case Direction::Right:
            direction_ = Direction::Up;
            break;
    }
}

void Snake::right()
{
    if (gameOver_) {
        reset();
        gameOver_ = false;
        return;
    }
    if (directionChanged_) {
        return;
    }
    directionChanged_ = true;
    switch (direction_) {
        case Direction::Up:
            direction_ = Direction::Right;
            break;
        case Direction::Left:
            direction_ = Direction::Up;
            break;
        case Direction::Down:
            direction_ = Direction::Left;
            break;
        case Direction::Right:
            direction_ = Direction::Down;
            break;
    }
}

Snake::Segment Snake::performStep(Segment segment, Direction direction)
{
    Segment res = segment;
    switch (direction) {
        case Direction::Up:
            --res.y;
            break;
        case Direction::Down:
            ++res.y;
            break;
        case Direction::Left:
            --res.x;
            break;
        case Direction::Right:
            ++res.x;
            break;
    }
    return res;
}

void Snake::spawnApple()
{
    if (!display_) {
        return;
    }
    auto trySpawn = [this] { apple_ = {rand() % (width_ - 2) + 1, rand() % (height_ - 2) + 1}; };
    trySpawn();
    for (;;) {
        if (!std::any_of(segments_.begin(), segments_.end(), [this](const Segment& segment) {
                return segment.x == apple_.x && segment.y == apple_.y;
            })) {
            break;
        }
        trySpawn();
    }
    display_->rounded_rectangle(apple_.x * blockWidth_, apple_.y * blockHeight_,
        (apple_.x + 1) * blockWidth_, (apple_.y + 1) * blockHeight_, 4, Color::GREEN, true);
}

void Snake::erase(Segment segment)
{
    if (!display_) {
        return;
    }
    display_->rectangle(segment.x * blockWidth_, segment.y * blockHeight_,
        (segment.x + 1) * blockWidth_, (segment.y + 1) * blockHeight_, Color::BLACK, true);
}

void Snake::draw(Segment segment)
{
    if (!display_) {
        return;
    }
    display_->rounded_rectangle(segment.x * blockWidth_ + 2, segment.y * blockHeight_ + 2,
        (segment.x + 1) * blockWidth_ - 2, (segment.y + 1) * blockHeight_ - 2, 3, Color::RED, true);
}

void Snake::reset()
{
    if (!display_) {
        return;
    }
    direction_ = Direction::Right;
    directionChanged_ = false;
    segments_.clear();
    display_->clear();
    for (int i = 0; i <= width_; ++i) {
        display_->rectangle(
            i * blockWidth_, 0, (i + 1) * blockWidth_, blockHeight_, hagl_color(10, 50, 255), true);
        display_->rectangle(i * blockWidth_, (height_ - 1) * blockHeight_, (i + 1) * blockWidth_,
            height_ * blockHeight_, hagl_color(10, 50, 255), true);
    }
    for (int i = 1; i < height_ - 1; ++i) {
        display_->rectangle(0, i * blockHeight_, blockWidth_, (i + 1) * blockHeight_,
            hagl_color(10, 50, 255), true);
        display_->rectangle(width_ * blockWidth_, i * blockHeight_, (width_ + 1) * blockWidth_,
            (i + 1) * blockHeight_, hagl_color(10, 50, 255), true);
    }
    segments_.push_back({5, 5});
    segments_.push_back({4, 5});
    segments_.push_back({3, 5});
    for (auto& segment : segments_) {
        draw(segment);
    }
    spawnApple();
}

} // namespace mini_lcd