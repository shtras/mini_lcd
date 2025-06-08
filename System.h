#pragma once

#include "Display.h"
#include "Functions/PerfGraph.h"
#include "Functions/Snake.h"
#include "Components/Button.h"
#include "Components/Encoder.h"

#include <array>

namespace mini_lcd
{
enum class Function {
    None,
    ColorTest,
    CPUGraph,
    MiscGraph,
    Snake,
    Settings,
};

class System
{
public:
    std::array<Function, 4> DisplayFunctions = {
        Function::None, Function::None, Function::None, Function::None};

    System();
    void Init(std::array<Display*, 4> displays);
    void Process();
    void OnMessage(Message& msg);

private:
    void setDisplayFunction(int idx, Function finction);

    std::array<Display*, 4> displays_ = {nullptr, nullptr, nullptr, nullptr};
    PerfGraph perfGraph_;
    Snake snake_;

    std::array<Button, 4> buttons_;
    std::array<Encoder, 2> encoders_;
};
} // namespace mini_lcd
