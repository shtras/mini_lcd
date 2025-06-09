#pragma once

#include "Display.h"

#include "ino_compat.h"

#include <list>

namespace mini_lcd
{
class Tetris
{
public:
    enum class Tetramino { I, J, L, O, S, T, Z };
    void SetDisplay(Display* display);
    void Process();
    void Left();
    void Right();
    void Rotate();
    void Drop();

private:
    void drawOccupied();
    void drawPiece(hagl_color_t color);
    void drawSquare(int x, int y, hagl_color_t color);
    void spawnPiece();
    void movePiece(int dx);
    bool advancePiece();
    bool isOccupied(int x, int y);
    void reset();
    void removeLines();

    Tetramino currentPiece_ = Tetramino::I;
    int x_ = 0;
    int y_ = 0;
    int rotation_ = 0;
    std::list<std::pair<int, int>> occupied_;
    static constexpr int blockSize_ = 10;
    static constexpr int width_ = Display::width / blockSize_;
    static constexpr int height_ = Display::height / blockSize_;
    std::array<int, height_> presentBlocks_;
    Display* display_ = nullptr;
    Timestamp last_time_ = 0;
    Timestamp speed_ = 500;
    bool gameOver_ = true;
};
} // namespace mini_lcd
