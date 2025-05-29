#include "PerfGraph.h"

#include <hagl.h>
#include <fonts.h>

#include <cstring>
#include <algorithm>
#include <sstream>

namespace mini_lcd
{

namespace
{
std::array<uint16_t, 16> colors = {hagl_color(250, 20, 20), hagl_color(20, 250, 20),
    hagl_color(20, 20, 250), hagl_color(220, 70, 70), hagl_color(70, 220, 70),
    hagl_color(70, 70, 220), hagl_color(180, 180, 10), hagl_color(180, 10, 180),
    hagl_color(10, 180, 180), hagl_color(140, 138, 110), hagl_color(150, 125, 100),
    hagl_color(160, 112, 90), hagl_color(170, 99, 80), hagl_color(180, 86, 70),
    hagl_color(190, 73, 60), hagl_color(200, 60, 50)};
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
    hagl_fill_rectangle(*display, bezelX, bezelTop, display->width - bezelX - 1,
        display->height - bezelBottom, hagl_color(6, 6, 30));
    display->hline(bezelX, bezelTop, display->width - bezelX * 2, hagl_color(12, 163, 196));
    display->vline(
        bezelX, bezelTop, display->height - bezelTop - bezelBottom, hagl_color(12, 163, 196));
    display->vline(display->width - bezelX - 1, bezelTop, display->height - bezelTop - bezelBottom,
        hagl_color(12, 163, 196));
}

void PerfGraph::drawCPU()
{
    drawGraphFrame(cpuDisplay_);

    constexpr float stretchX = cpuDisplay_->width / static_cast<float>(kMaxCpuDataPoints + 1);

    for (uint32_t i = 0; i < kMaxCpuDataPoints - 1; ++i) {
        int idx1 = (cpuStartIndex_ + i) % kMaxCpuDataPoints;
        int idx2 = (cpuStartIndex_ + i + 1) % kMaxCpuDataPoints;

        for (int gpuIdx = 0; gpuIdx < 16; ++gpuIdx) {
            uint32_t cpu1 = cpuData_[idx1][gpuIdx] * 1.5;
            uint32_t cpu2 = cpuData_[idx2][gpuIdx] * 1.5;
            hagl_draw_line(*cpuDisplay_, (i + 1) * stretchX, 155 - cpu1, (i + 2) * stretchX,
                155 - cpu2, colors[gpuIdx]);
        }
    }

    for (int gpuIdx = 0; gpuIdx < 4; ++gpuIdx) {
        std::wstring s =
            std::to_wstring(
                cpuData_[(cpuStartIndex_ + kMaxCpuDataPoints - 1) % kMaxCpuDataPoints][gpuIdx]) +
            L" %";
        hagl_put_text(
            *cpuDisplay_, s.c_str(), 10, (gpuIdx + 1) * 9, hagl_color(5, 255, 60), Fonts::font5x8);
    }
}

void PerfGraph::drawMisc()
{
    auto lastIndex = (miscStartIndex_ + kMaxMiscDataPoints - 1) % kMaxMiscDataPoints;
    drawGraphFrame(miscDisplay_);
    std::wstringstream ss;
    ss << "RAM: " << 64.0f - ramData_[lastIndex] / 1024.0f << " GB";
    hagl_put_text(
        *miscDisplay_, ss.str().c_str(), 10, 10, hagl_color(255, 255, 255), Fonts::font5x8);
    ss.str(std::wstring());
    ss << "GPU: " << gpuData_[lastIndex] << " %";
    hagl_put_text(
        *miscDisplay_, ss.str().c_str(), 10, 20, hagl_color(255, 255, 255), Fonts::font5x8);
    ss.str(std::wstring());
    ss << "GPUVD: " << gpuvd_ << " %";
    hagl_put_text(
        *miscDisplay_, ss.str().c_str(), 10, 30, hagl_color(255, 255, 255), Fonts::font5x8);
    ss.str(std::wstring());
    ss << "GPUVE: " << gpuve_ << " %";
    hagl_put_text(
        *miscDisplay_, ss.str().c_str(), 10, 40, hagl_color(255, 255, 255), Fonts::font5x8);
    ss.str(std::wstring());
    ss << "GPUMEM: " << gpumem_ / 1024.0 << " GB";
    hagl_put_text(
        *miscDisplay_, ss.str().c_str(), 10, 50, hagl_color(255, 255, 255), Fonts::font5x8);
    ss.str(std::wstring());

    constexpr float stretchX = cpuDisplay_->width / 2 / static_cast<float>(kMaxMiscDataPoints + 1);
    constexpr int graphHeight = 70;

    hagl_fill_rectangle(*miscDisplay_, 2, miscDisplay_->height - graphHeight,
        miscDisplay_->width / 2 - 1, miscDisplay_->height, hagl_color(26, 26, 10));

    hagl_fill_rectangle(*miscDisplay_, miscDisplay_->width / 2 + 2,
        miscDisplay_->height - graphHeight, miscDisplay_->width - 2, miscDisplay_->height,
        hagl_color(26, 26, 10));

    for (uint32_t i = 0; i < kMaxMiscDataPoints - 1; ++i) {
        int idx1 = (miscStartIndex_ + i) % kMaxMiscDataPoints;
        int idx2 = (miscStartIndex_ + i + 1) % kMaxMiscDataPoints;

        uint32_t gpu1 = gpuData_[idx1] * (miscDisplay_->height / static_cast<float>(graphHeight));
        uint32_t gpu2 = gpuData_[idx2] * (miscDisplay_->height / static_cast<float>(graphHeight));
        hagl_draw_line(*miscDisplay_, (i + 1) * stretchX, 155 - gpu1, (i + 2) * stretchX,
            155 - gpu2, colors[3]);

        uint32_t ram1 = ramData_[idx1] / 1024.0f / 64.0f * static_cast<float>(graphHeight);
        uint32_t ram2 = ramData_[idx2] / 1024.0f / 64.0f * static_cast<float>(graphHeight);

        hagl_draw_line(*miscDisplay_, (i + 1) * stretchX + miscDisplay_->width / 2, 155 - ram1,
            (i + 2) * stretchX + miscDisplay_->width / 2, 155 - ram2, colors[2]);
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
