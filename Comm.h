#pragma once

#include <map>
#include <cstdint>
#include <memory>
#include <array>
#include <optional>

namespace mini_lcd
{

struct Message
{
    enum class Type : uint32_t { Unknown, Measurements };
    Type type = Type::Unknown;
    std::array<uint32_t, 32> data;

    static const std::map<Message::Type, int> Size;
};

class Receiver
{
public:
    Message* process();

private:
    enum class State { Idle, Receiving };
    uint64_t idleTimeoutMs_ = 5;
    State state_ = State::Idle;
    Message message_;
    int received_ = 0;
    int toReceive_ = 0;
};
} // namespace mini_lcd
