#pragma once

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

struct Settings {
    std::array<Function, 4> DisplayFunctions = {Function::None, Function::None, Function::None, Function::None};
};
}
