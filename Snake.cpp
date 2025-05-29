#include "Snake.h"
#include <hagl_hal.h>
#include <hagl.h>

#include <algorithm>

namespace mini_lcd
{
Snake::Snake(Display& display)
    : display_(display)
    , width_(display.width / 10)
    , height_(display.height / 10)
{
    reset();
}

void Snake::process()
{
    auto now = Utils::millis();
    if (now - last_time_ < speed_) {
        return;
    }
    directionChanged_ = false;
    last_time_ = now;
    Segment nextHead = performStep(segments_.front(), direction_);

    if (nextHead.x < 1 || nextHead.x > width_ - 1 || nextHead.y < 1 || nextHead.y > height_ - 2 ||
        std::any_of(segments_.begin(), segments_.end(), [nextHead](const Segment& segment) {
            return nextHead.x == segment.x && nextHead.y == segment.y;
        })) {
        reset();
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
    hagl_fill_rounded_rectangle(display_, apple_.x * blockWidth_, apple_.y * blockHeight_,
        (apple_.x + 1) * blockWidth_, (apple_.y + 1) * blockHeight_, 4, hagl_color(0, 255, 0));
}

void Snake::erase(Segment segment)
{
    hagl_fill_rectangle(display_, segment.x * blockWidth_, segment.y * blockHeight_,
        (segment.x + 1) * blockWidth_, (segment.y + 1) * blockHeight_, hagl_color(0, 0, 0));
}

void Snake::draw(Segment segment)
{
    hagl_fill_rounded_rectangle(display_, segment.x * blockWidth_ + 2, segment.y * blockHeight_ + 2,
        (segment.x + 1) * blockWidth_ - 2, (segment.y + 1) * blockHeight_ - 2, 3,
        hagl_color(255, 0, 0));
}

void Snake::reset()
{
    direction_ = Direction::Right;
    directionChanged_ = false;
    segments_.clear();
    hagl_clear(display_);
    for (int i = 0; i <= width_; ++i) {
        hagl_fill_rectangle(display_, i * blockWidth_, 0, (i + 1) * blockWidth_, blockHeight_,
            hagl_color(10, 50, 255));
        hagl_fill_rectangle(display_, i * blockWidth_, (height_ - 1) * blockHeight_,
            (i + 1) * blockWidth_, height_ * blockHeight_, hagl_color(10, 50, 255));
    }
    for (int i = 1; i < height_ - 1; ++i) {
        hagl_fill_rectangle(display_, 0, i * blockHeight_, blockWidth_, (i + 1) * blockHeight_,
            hagl_color(10, 50, 255));
        hagl_fill_rectangle(display_, width_ * blockWidth_, i * blockHeight_,
            (width_ + 1) * blockWidth_, (i + 1) * blockHeight_, hagl_color(10, 50, 255));
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