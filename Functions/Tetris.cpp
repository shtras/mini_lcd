#include "Tetris.h"
#include "fonts.h"

#include <array>
#include <map>
#include <cstdlib>

namespace mini_lcd
{

namespace
{
struct Offset
{
    int x;
    int y;
};

std::map<Tetris::Tetramino, std::array<Offset, 4>> tetraminoOffsets = {
    {Tetris::Tetramino::I, {{Offset{-1, 0}, Offset{0, 0}, Offset{1, 0}, Offset{2, 0}}}},
    {Tetris::Tetramino::J, {{Offset{-1, 0}, Offset{0, 0}, Offset{1, 0}, Offset{1, 1}}}},
    {Tetris::Tetramino::L, {{Offset{0, 0}, Offset{1, 0}, Offset{2, 0}, Offset{0, 1}}}},
    {Tetris::Tetramino::O, {{Offset{0, 0}, Offset{1, 0}, Offset{0, 1}, Offset{1, 1}}}},
    {Tetris::Tetramino::S, {{Offset{-1, 1}, Offset{0, 1}, Offset{0, 0}, Offset{1, 0}}}},
    {Tetris::Tetramino::T, {{Offset{-1, 0}, Offset{0, 0}, Offset{1, 0}, Offset{0, 1}}}},
    {Tetris::Tetramino::Z, {{Offset{-1, 0}, Offset{0, 0}, Offset{0, 1}, Offset{1, 1}}}},
};

std::map<Tetris::Tetramino, hagl_color_t> tetraminoColors = {
    {Tetris::Tetramino::I, Color::CYAN},
    {Tetris::Tetramino::J, Color::BLUE},
    {Tetris::Tetramino::L, Color::ORANGE},
    {Tetris::Tetramino::O, Color::YELLOW},
    {Tetris::Tetramino::S, Color::GREEN},
    {Tetris::Tetramino::T, Color::PURPLE},
    {Tetris::Tetramino::Z, Color::RED},
};

void applyOffset(Tetris::Tetramino piece, int rotation, const Offset& offset, int& x, int& y)
{
    switch (rotation) {
        case 0:
            x += offset.x;
            y += offset.y;
            break;
        case 1:
            x += offset.y;
            y -= offset.x;
            break;
        case 2:
            x -= offset.x;
            y -= offset.y;
            break;
        case 3:
            x -= offset.y;
            y += offset.x;
            break;
    }
}
} // namespace

void Tetris::SetDisplay(Display* display)
{
    if (display_) {
        display_->clear();
    }
    display_ = display;
}

void Tetris::Left()
{
    if (!display_) {
        return;
    }
    if (gameOver_) {
        gameOver_ = false;
        reset();
        return;
    }
    drawPiece(Color::BLACK);
    movePiece(-1);
    drawPiece(tetraminoColors.at(currentPiece_));
}

void Tetris::Right()
{
    if (!display_) {
        return;
    }
    if (gameOver_) {
        gameOver_ = false;
        reset();
        return;
    }
    drawPiece(Color::BLACK);
    movePiece(1);
    drawPiece(tetraminoColors.at(currentPiece_));
}

void Tetris::Process()
{
    if (!display_ || gameOver_) {
        return;
    }
    auto now = millis();
    if (now - last_time_ < speed_) {
        return;
    }
    last_time_ = now;
    drawPiece(Color::BLACK);
    advancePiece();
    if (gameOver_) {
        return;
    }
    drawPiece(tetraminoColors.at(currentPiece_));
}

void Tetris::Rotate()
{
    if (!display_) {
        return;
    }
    auto newRotation = (rotation_ + 1) % 4;
    const auto& offsets = tetraminoOffsets[currentPiece_];
    for (const auto& offset : offsets) {
        int x = x_;
        int y = y_;
        applyOffset(currentPiece_, newRotation, offset, x, y);
        if (x < 0 || x >= width_ || y < 0 || y >= height_) {
            return;
        }
    }
    drawPiece(Color::BLACK);
    rotation_ = newRotation;
    drawPiece(tetraminoColors.at(currentPiece_));
}

void Tetris::Drop()
{
    if (!display_) {
        return;
    }
    drawPiece(Color::BLACK);
    while (advancePiece())
        ;
    if (!gameOver_) {
        drawPiece(tetraminoColors.at(currentPiece_));
    }
}

void Tetris::reset()
{
    display_->clear();
    presentBlocks_.fill(0);
    occupied_.clear();
    spawnPiece();
}

void Tetris::drawSquare(int x, int y, hagl_color_t color)
{
    display_->rounded_rectangle(x * blockSize_ + 1, y * blockSize_ + 1, (x + 1) * blockSize_ - 2,
        (y + 1) * blockSize_ - 2, 11, color, true);
}

void Tetris::drawPiece(hagl_color_t color)
{
    const auto& offsets = tetraminoOffsets[currentPiece_];
    for (const auto& offset : offsets) {
        int x = x_;
        int y = y_;
        applyOffset(currentPiece_, rotation_, offset, x, y);
        if (x < 0 || x >= width_ || y < 0 || y >= height_) {
            continue;
        }
        drawSquare(x, y, color);
    }
}

void Tetris::drawOccupied()
{
    for (const auto& occupied : occupied_) {
        drawSquare(occupied.first, occupied.second, Color::GRAY);
    }
}

bool Tetris::isOccupied(int x, int y)
{
    if (x < 0 || x >= width_ || y >= height_) {
        return true;
    }
    for (const auto& occupied : occupied_) {
        if (occupied.first == x && occupied.second == y) {
            return true;
        }
    }
    return false;
}

void Tetris::spawnPiece()
{
    currentPiece_ = static_cast<Tetramino>(rand() % 7);
    x_ = width_ / 2 - 1;
    y_ = 0;
    rotation_ = 0;
    for (const auto& occupied : occupied_) {
        if (occupied.first == x_ && occupied.second == y_) {
            gameOver_ = true;
            display_->text(L"Game Over", 10, 10, Fonts::font6x9, Color::RED);
            return;
        }
    }
    drawPiece(tetraminoColors.at(currentPiece_));
}

void Tetris::movePiece(int dx)
{
    const auto& offsets = tetraminoOffsets[currentPiece_];
    for (const auto& offset : offsets) {
        int x = x_ + dx;
        int y = y_;
        applyOffset(currentPiece_, rotation_, offset, x, y);
        if (isOccupied(x, y)) {
            return;
        }
    }
    x_ += dx;
}

bool Tetris::advancePiece()
{
    const auto& offsets = tetraminoOffsets[currentPiece_];
    for (const auto& offset : offsets) {
        int x = x_;
        int y = y_ + 1;
        applyOffset(currentPiece_, rotation_, offset, x, y);
        if (isOccupied(x, y)) {
            for (const auto& offset : offsets) {
                int occupiedX = x_;
                int occupiedY = y_;
                applyOffset(currentPiece_, rotation_, offset, occupiedX, occupiedY);
                occupied_.emplace_back(occupiedX, occupiedY);
                presentBlocks_[occupiedY]++;
            }
            drawOccupied();
            spawnPiece();
            if (!gameOver_) {
                removeLines();
            }
            return false;
        }
    }
    ++y_;
    return true;
}

void Tetris::removeLines()
{
    bool needRedraw = false;
    for (int i = 0; i < height_; ++i) {
        if (presentBlocks_[i] == width_) {
            for (auto it = occupied_.begin(); it != occupied_.end();) {
                if (it->second == i) {
                    it = occupied_.erase(it);
                    needRedraw = true;
                } else {
                    if (it->second < i) {
                        it->second++;
                    }
                    ++it;
                }
            }
        }
    }
    if (needRedraw) {
        presentBlocks_.fill(0);
        for (const auto& occupied : occupied_) {
            presentBlocks_[occupied.second]++;
        }
        display_->clear();
        drawOccupied();
    }
}

} // namespace mini_lcd
