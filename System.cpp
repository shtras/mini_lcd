#include "System.h"
#include "Utils/Logger.h"
#include "fonts.h"

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
} // namespace

namespace mini_lcd
{
System::System()
:
buttons_{
    Button(18),
    Button(19),
    Button(26),
    Button(27),
},
encoders_{
    Encoder(4, 5, 28),
    Encoder(20, 21, 22)
}
{
    encoders_[0].setOnLeft([]() { Logger::info() << "Encoder: Left\n"; });
    encoders_[0].setOnRight([]() { Logger::info() << "Encoder: Right\n"; });

    buttons_[2].SetOnDown([this] { snake_.left(); });
    buttons_[3].SetOnDown([this] { snake_.right(); });
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
    auto currentFunction = DisplayFunctions[idx];
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
        default:
            Logger::error() << "Unknown function: " << static_cast<int>(currentFunction);
            break;
    }

    DisplayFunctions[idx] = function;
}
} // namespace mini_lcd
