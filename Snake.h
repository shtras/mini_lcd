#pragma once
#include "Display.h"
#include "Utils.h"

#include <list>

namespace mini_lcd
{
class Snake
{
public:
    Snake(Display& display);

    void process();
    void left();
    void right();

private:
    enum class Direction { Up, Down, Left, Right };
    struct Segment {
        int x;
        int y;
    };

    Segment performStep(Segment segment, Direction direction);
    void erase(Segment segment);
    void draw(Segment segment);
    void reset();
    void spawnApple();

    Display& display_;
    uint64_t last_time_ = Utils::millis();
    uint64_t speed_ = 500;
    Direction direction_ = Direction::Right;

    std::list<Segment> segments_;
    Segment apple_{-1, -1};
    const int width_ = 0;
    const int height_ = 0;
    const int blockWidth_ = 10;
    const int blockHeight_ = 10;
    bool directionChanged_ = false;
};
} // namespace mini_lcd