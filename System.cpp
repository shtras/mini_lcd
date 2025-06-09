#include "System.h"
#include "Utils/Logger.h"
#include "fonts.h"

#include <hardware/watchdog.h>

#include <vector>
#include <string>

namespace
{
void colorTest(Display& display)
{
    display.clear();
    constexpr auto width = 20;
    constexpr auto height = 20;

    constexpr auto numX = display.width / width;
    constexpr auto numY = display.height / height;

    for (int y = 0; y < numY; ++y) {
        for (int x = 0; x < numX; ++x) {
            auto colorIdx = (y * numX + x) % Color::colors.size();
            display.rectangle(x * width, y * height, (x + 1) * width - 1, (y + 1) * height - 1,
                Color::colors[colorIdx], true);
            display.text(std::to_wstring(colorIdx).c_str(), x * width + 1, y * height + 1,
                Fonts::font5x7, Color::WHITE);
        }
    }
}

std::vector<std::wstring> MainMenuItems = {
    L"Display functions",
    L"Logger verbosity",
    L"Reboot",
    L"Cancel",
};

std::vector<std::wstring> DisplayNames = {
    L"Top Left",
    L"Top Right",
    L"Bottom Left",
    L"Bottom Right",
};

std::vector<std::wstring> FunctionNames = {
    L"None",
    L"Color Test",
    L"CPU Graph",
    L"Misc Graph",
    L"Snake",
    L"Settings",
};

std::vector<std::wstring> LoggerVerbosityNames = {
    L"Trace",
    L"Debug",
    L"Info",
    L"Warn",
    L"Error",
};
} // namespace

namespace mini_lcd
{
System::System()
: buttons_{
    Button(19),
    Button(26),
    Button(27),
    Button(18),
}
, encoders_{
    Encoder(5, 4, 28),
    Encoder(20, 21, 22),
}
{
    encoders_[0].SetOnLeft([]() { Logger::debug() << "Encoder 0: Left\n"; });
    encoders_[0].SetOnRight([]() { Logger::debug() << "Encoder 0: Right\n"; });
    encoders_[1].SetOnLeft([this]() { menu_.Up(); });
    encoders_[1].SetOnRight([this]() { menu_.Down(); });
    encoders_[1].SetOnPress([this]() {
        if (settingsDisplay_ != -1 && displayFunctions[settingsDisplay_] == Function::Settings) {
            menu_.Click();
        } else {
            setDisplayFunction(3, Function::Settings);
        }
    });

    buttons_[0].SetOnUp([] { Logger::debug() << "Button 0 Up\n"; });
    buttons_[1].SetOnUp([] { Logger::debug() << "Button 1 Up\n"; });
    buttons_[2].SetOnUp([] { Logger::debug() << "Button 2 Up\n"; });
    buttons_[3].SetOnUp([] { Logger::debug() << "Button 3 Up\n"; });

    buttons_[2].SetOnDown([this] { snake_.Left(); });
    buttons_[3].SetOnDown([this] { snake_.Right(); });
}

void System::Init(std::array<Display*, 4> displays)
{
    displays_ = displays;
    setDisplayFunction(0, Function::MiscGraph);
    setDisplayFunction(1, Function::CPUGraph);
    setDisplayFunction(2, Function::ColorTest);
    setDisplayFunction(3, Function::Snake);
}

void System::Process()
{
    for (auto& button : buttons_) {
        button.Process();
    }
    for (auto& encoder : encoders_) {
        encoder.Process();
    }
    snake_.Process();
}

void System::OnMessage(Message& msg)
{
    if (msg.type == Message::Type::Measurements) {
        perfGraph_.AddData(msg);
        perfGraph_.Process();
    } else {
        Logger::error() << "Unknown message type: " << static_cast<int>(msg.type) << "\n";
    }
}

void System::setDisplayFunction(int idx, Function function)
{
    if (idx < 0 || idx > 3) {
        Logger::error() << "Invalid display index: " << idx << "\n";
        return;
    }
    Logger::debug() << "Setting function " << static_cast<int>(function) << " on display " << idx
                    << "\n";
    Logger::debug() << "Current functions: " << static_cast<int>(displayFunctions[0]) << ", "
                    << static_cast<int>(displayFunctions[1]) << ", "
                    << static_cast<int>(displayFunctions[2]) << ", "
                    << static_cast<int>(displayFunctions[3]) << "\n";
    if (function != Function::None) {
        for (int i = 0; i < 4; ++i) {
            if (displayFunctions[i] == function) {
                if (i == idx) {
                    return;
                }
                Logger::info() << "Function " << static_cast<int>(function)
                               << " already set on display " << i << "\n";
                setDisplayFunction(i, Function::None);
            }
        }
    }
    auto currentFunction = displayFunctions[idx];
    auto display = displays_[idx];
    switch (currentFunction) {
        case Function::None:
            break;
        case Function::ColorTest:
            display->clear();
            break;
        case Function::CPUGraph:
            perfGraph_.SetCpuDisplay(nullptr);
            break;
        case Function::MiscGraph:
            perfGraph_.SetMiscDisplay(nullptr);
            break;
        case Function::Snake:
            snake_.SetDisplay(nullptr);
            break;
        case Function::Settings:
            display->clear();
            break;
        default:
            Logger::error() << "Unknown function: " << static_cast<int>(currentFunction);
            break;
    }

    switch (function) {
        case Function::None:
            display->clear();
            break;
        case Function::ColorTest:
            colorTest(*display);
            break;
        case Function::CPUGraph:
            perfGraph_.SetCpuDisplay(display);
            break;
        case Function::MiscGraph:
            perfGraph_.SetMiscDisplay(display);
            break;
        case Function::Snake:
            snake_.SetDisplay(display);
            break;
        case Function::Settings:
            lastSettingFunction_ = currentFunction;
            settingsDisplay_ = idx;
            showSettings();
            break;
        default:
            Logger::error() << "Unknown function: " << static_cast<int>(currentFunction);
            break;
    }

    displayFunctions[idx] = function;
}

void System::showSettings()
{
    menu_.SetDisplay(displays_[settingsDisplay_]);
    menu_.SetOnSelect([this](int idx) { onMainMenuItem(idx); });
    menu_.SetItems(&MainMenuItems);
}

void System::onMainMenuItem(int idx)
{
    switch (idx) {
        case 0:
            showDisplayNames();
            break;
        case 1:
            showVerbosityNames();
            break;
        case 2:
            watchdog_reboot(0, 0, 0);
            break;
        default:
            closeSettings();
            break;
    }
}

void System::showDisplayNames()
{
    menu_.SetOnSelect([this](int idx) {
        selectedDisplay_ = idx;
        showFunctionNames();
    });
    menu_.SetItems(&DisplayNames);
}

void System::showFunctionNames()
{
    menu_.SetOnSelect([this](int idx) {
        if (idx < 0 || idx >= static_cast<int>(FunctionNames.size())) {
            Logger::error() << "Invalid function index: " << idx << "\n";
            return;
        }
        if (lastSettingFunction_ == static_cast<Function>(idx)) {
            lastSettingFunction_ = Function::None;
        }
        closeSettings();
        setDisplayFunction(selectedDisplay_, static_cast<Function>(idx));
        selectedDisplay_ = -1;
    });
    menu_.SetItems(&FunctionNames);
}

void System::showVerbosityNames()
{
    menu_.SetOnSelect([this](int idx) {
        if (idx < 0 || idx >= static_cast<int>(LoggerVerbosityNames.size())) {
            Logger::error() << "Invalid verbosity index: " << idx << "\n";
            return;
        }
        Logger::GetLogger().SetVerbosity(static_cast<Logger::Verbosity>(idx));
        closeSettings();
    });
    menu_.SetItems(&LoggerVerbosityNames);
}

void System::closeSettings()
{
    setDisplayFunction(settingsDisplay_, lastSettingFunction_);
    lastSettingFunction_ = Function::None;
    settingsDisplay_ = -1;
}
} // namespace mini_lcd
