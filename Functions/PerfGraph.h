#pragma once
#include "Display.h"
#include "Utils/Comm.h"
#include "ino_compat.h"

namespace mini_lcd
{
class PerfGraph
{
public:
    explicit PerfGraph(Display* cpuDisplay, Display* miscDisplay);
    void SetCpuDisplay(Display* display);
    void SetMiscDisplay(Display* display);
    void AddData(Message& msg);

    void Draw();

private:
    void drawCPU();
    void drawMisc();

    Display* cpuDisplay_ = nullptr;
    Display* miscDisplay_ = nullptr;

    static constexpr int kMaxCpuDataPoints = 50;
    static constexpr int kMaxMiscDataPoints = 35;
    std::array<std::array<uint32_t, 17>, kMaxCpuDataPoints> cpuData_;
    std::array<uint32_t, kMaxMiscDataPoints> gpuData_;
    std::array<uint32_t, kMaxMiscDataPoints> ramData_;
    uint32_t cpuStartIndex_ = 0;
    uint32_t miscStartIndex_ = 0;
    Timestamp lastUpdate_ = 0;
    uint32_t gpuvd_ = 0;
    uint32_t gpuve_ = 0;
    uint32_t gpumem_ = 0;
};
} // namespace mini_lcd
