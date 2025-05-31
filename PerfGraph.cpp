#include "PerfGraph.h"

#include <hagl.h>
#include <fonts.h>

#include <cstring>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace mini_lcd
{

namespace
{
std::array<uint16_t, 16> colors = {Color::RED, Color::GREEN, Color::BLUE, Color::YELLOW,
    Color::CYAN, Color::MAGENTA, Color::ORANGE, Color::PURPLE, Color::PINK, Color::BROWN,
    Color::DARK_GRAY, Color::DARK_GRAY, Color::DARK_GRAY, Color::DARK_GRAY,
    Color::DARK_GRAY, Color::DARK_GRAY};
}

PerfGraph::PerfGraph(Display* cpuDisplay, Display* miscDisplay)
    : cpuDisplay_(cpuDisplay)
    , miscDisplay_(miscDisplay)
{
    for (auto& point : cpuData_) {
        point.fill(0);
    }
    for (auto& point : gpuData_) {
        point = 0;
    }
    for (auto& point : ramData_) {
        point = 0;
    }
}

void PerfGraph::AddData(Message& msg)
{
    assert(msg.type == Message::Type::Measurements);
    std::memcpy(cpuData_[cpuStartIndex_].data(), msg.data.data(), 16 * sizeof(uint32_t));
    std::sort(cpuData_[cpuStartIndex_].begin(), cpuData_[cpuStartIndex_].end(),
        [](uint32_t a, uint32_t b) { return a > b; });

    ramData_[miscStartIndex_] = msg.data[16];
    gpuData_[miscStartIndex_] = msg.data[17];
    gpuvd_ = msg.data[18];
    gpuve_ = msg.data[19];
    gpumem_ = msg.data[20];

    cpuStartIndex_ = (cpuStartIndex_ + 1) % kMaxCpuDataPoints;
    miscStartIndex_ = (miscStartIndex_ + 1) % kMaxMiscDataPoints;
}

void drawGraphFrame(Display* display)
{
    constexpr int bezelX = 0;
    constexpr int bezelTop = 5;
    constexpr int bezelBottom = 5;
    display->rectangle(bezelX, bezelTop, display->width - bezelX - 1, display->height - bezelBottom,
        hagl_color(6, 6, 30), true);
    display->hline(bezelX, bezelTop, display->width - bezelX * 2, hagl_color(12, 163, 196));
    display->vline(
        bezelX, bezelTop, display->height - bezelTop - bezelBottom, hagl_color(12, 163, 196));
    display->vline(display->width - bezelX - 1, bezelTop, display->height - bezelTop - bezelBottom,
        hagl_color(12, 163, 196));
}

void PerfGraph::drawCPU()
{
    auto disp = cpuDisplay_;
    drawGraphFrame(disp);

    constexpr float stretchX = disp->width / static_cast<float>(kMaxCpuDataPoints + 1);
    float mean = 0;
    for (uint32_t i = 0; i < kMaxCpuDataPoints - 1; ++i) {
        int idx1 = (cpuStartIndex_ + i) % kMaxCpuDataPoints;
        int idx2 = (cpuStartIndex_ + i + 1) % kMaxCpuDataPoints;

        for (int gpuIdx = 15; gpuIdx >= 0; --gpuIdx) {
            uint32_t cpu1 = cpuData_[idx1][gpuIdx] * 1.5;
            uint32_t cpu2 = cpuData_[idx2][gpuIdx] * 1.5;
            disp->line(
                (i + 1) * stretchX, 155 - cpu1, (i + 2) * stretchX, 155 - cpu2, colors[gpuIdx]);
            if (i == kMaxCpuDataPoints - 2) {
                mean += cpuData_[idx2][gpuIdx];
            }
        }
    }
    auto& lastPoint = cpuData_[(cpuStartIndex_ + kMaxCpuDataPoints - 1) % kMaxCpuDataPoints];
    mean /= 16.f;
    std::wstring top = L"Top: " + std::to_wstring(lastPoint[0]) + L" %";
    std::wstring median = L"Median: " + std::to_wstring(lastPoint[8]) + L" %";
    std::wstring meanStr = L"Mean: " + std::to_wstring(static_cast<int>(mean)) + L" %";
    std::wstring bottom = L"Bottom: " + std::to_wstring(lastPoint[15]) + L" %";

    disp->text(top.c_str(), 10, 10, Color::GREEN, Fonts::font5x8);
    disp->text(median.c_str(), 10, 20, Color::GREEN, Fonts::font5x8);
    disp->text(meanStr.c_str(), 10, 30, Color::GREEN, Fonts::font5x8);
    disp->text(bottom.c_str(), 10, 40, Color::GREEN, Fonts::font5x8);
}

void PerfGraph::drawMisc()
{
    auto disp = miscDisplay_;
    auto lastIndex = (miscStartIndex_ + kMaxMiscDataPoints - 1) % kMaxMiscDataPoints;
    drawGraphFrame(disp);
    std::wstringstream ss;
    ss << "RAM: " << std::fixed << std::setprecision(2) << 64.0f - ramData_[lastIndex] / 1024.0f
       << " GB";
    disp->text(ss.str().c_str(), 10, 10, Color::WHITE, Fonts::font5x8);
    ss.str(std::wstring());
    ss << "GPU: " << gpuData_[lastIndex] << " %";
    disp->text(ss.str().c_str(), 10, 20, Color::WHITE, Fonts::font5x8);
    ss.str(std::wstring());
    ss << "GPUVD: " << gpuvd_ << " %";
    disp->text(ss.str().c_str(), 10, 30, Color::WHITE, Fonts::font5x8);
    ss.str(std::wstring());
    ss << "GPUVE: " << gpuve_ << " %";
    disp->text(ss.str().c_str(), 10, 40, Color::WHITE, Fonts::font5x8);
    ss.str(std::wstring());
    ss << "GPUMEM: " << std::fixed << std::setprecision(2) << gpumem_ / 1024.0 << " GB";
    disp->text(ss.str().c_str(), 10, 50, Color::WHITE, Fonts::font5x8);
    ss.str(std::wstring());

    constexpr float stretchX = cpuDisplay_->width / 2 / static_cast<float>(kMaxMiscDataPoints + 1);
    constexpr int graphHeight = 70;

    disp->rectangle(2, disp->height - graphHeight, disp->width / 2 - 1, disp->height,
        hagl_color(26, 26, 10), true);

    disp->rectangle(disp->width / 2 + 2, disp->height - graphHeight, disp->width - 2, disp->height,
        hagl_color(26, 26, 10), true);

    for (uint32_t i = 0; i < kMaxMiscDataPoints - 1; ++i) {
        int idx1 = (miscStartIndex_ + i) % kMaxMiscDataPoints;
        int idx2 = (miscStartIndex_ + i + 1) % kMaxMiscDataPoints;

        uint32_t gpu1 = gpuData_[idx1] * static_cast<float>(graphHeight) / 100.0f;
        uint32_t gpu2 = gpuData_[idx2] * static_cast<float>(graphHeight) / 100.0f;
        disp->line((i + 1) * stretchX, 155 - gpu1, (i + 2) * stretchX, 155 - gpu2, colors[3]);

        uint32_t ram1 = ramData_[idx1] / 1024.0f / 64.0f * static_cast<float>(graphHeight);
        uint32_t ram2 = ramData_[idx2] / 1024.0f / 64.0f * static_cast<float>(graphHeight);

        disp->line((i + 1) * stretchX + disp->width / 2, 155 - ram1,
            (i + 2) * stretchX + disp->width / 2, 155 - ram2, colors[2]);
    }
}

void PerfGraph::Draw()
{
    Timestamp now = millis();
    if (now - lastUpdate_ < 1000) {
        return; // Update only every 100 ms
    }
    lastUpdate_ = now;
    if (cpuDisplay_) {
        drawCPU();
    }
    if (miscDisplay_) {
        drawMisc();
    }
}
} // namespace mini_lcd
