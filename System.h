#pragma once

#include "Display.h"
#include "Functions/PerfGraph.h"
#include "Functions/Snake.h"
#include "Functions/Menu.h"
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
    System();
    void Init(std::array<Display*, 4> displays);
    void Process();
    void OnMessage(Message& msg);

private:
    void setDisplayFunction(int idx, Function finction);
    void showSettings();
    void showDisplayNames();
    void showFunctionNames();
    void showVerbosityNames();
    void onMainMenuItem(int idx);
    void closeSettings();

    std::array<Function, 4> displayFunctions = {
        Function::None, Function::None, Function::None, Function::None};
    std::array<Display*, 4> displays_ = {nullptr, nullptr, nullptr, nullptr};
    PerfGraph perfGraph_;
    Snake snake_;
    Menu menu_;

    std::array<Button, 4> buttons_;
    std::array<Encoder, 2> encoders_;
    int settingsDisplay_ = -1;
    Function lastSettingFunction_ = Function::None;
    int selectedDisplay_ = -1;
};
} // namespace mini_lcd
