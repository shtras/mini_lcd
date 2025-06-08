#pragma once

#include <map>
#include <cstdint>
#include <memory>
#include <array>
#include <optional>
#include <list>

namespace mini_lcd
{

struct Message
{
    enum class Type : uint32_t { Unknown, Measurements, Snake };
    Type type = Type::Unknown;
    std::array<uint32_t, 32> data;

    static const std::map<Message::Type, int> Size;
};

class Receiver
{
public:
    Message* Process();

private:
    enum class State { Idle, Receiving };
    uint64_t idleTimeoutMs_ = 5;
    State state_ = State::Idle;
    Message message_;
    int received_ = 0;
    int toReceive_ = 0;
};

class Sender
{
public:
    static Sender& GetInstance()
    {
        static Sender instance;
        return instance;
    }
    void Send(const Message& message);
    void Process();
private:
    Sender() = default;
    std::list<Message> messages_;
};
} // namespace mini_lcd
