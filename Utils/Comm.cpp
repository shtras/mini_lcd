#include "Comm.h"

#include <pico/stdlib.h>
#include <pico/multicore.h>

#include <iostream>
#include <cstring>

namespace mini_lcd
{
const std::map<Message::Type, int> Message::Size = {
    {Message::Type::Measurements, 21}, {Message::Type::Snake, 1}};

Message* Receiver::Process()
{
    if (!multicore_fifo_rvalid()) {
        return nullptr;
    }
    switch (state_) {
        case State::Idle: {
            uint32_t val = multicore_fifo_pop_blocking();
            Message::Type type = static_cast<Message::Type>(val);
            //std::cout << "Now receiving " << val << "\n";
            message_.type = type;
            if (Message::Size.count(type) == 0) {
                std::cout << "Bad message type: " << val << "\n";
                return nullptr;
            }
            toReceive_ = Message::Size.at(type);
            if (toReceive_ == 0) {
                return &message_;
            }
            state_ = State::Receiving;
            return nullptr;
        }
        case State::Receiving: {
            message_.data[received_++] = multicore_fifo_pop_blocking();
            //std::cout << "Received " << received_ << " out of " << toReceive_ << "\n";
            if (received_ >= toReceive_) {
                //std::cout << "Final\n";
                state_ = State::Idle;
                received_ = 0;
                return &message_;
            }
            break;
        }
    }
    return nullptr;
}

void Sender::Send(const Message& message)
{
    messages_.push_back(message);
}

void Sender::Process()
{
    if (messages_.empty()) {
        return;
    }
    auto& message = messages_.front();
    multicore_fifo_push_blocking(static_cast<uint32_t>(message.type));
    for (int i = 0; i < Message::Size.at(message.type); ++i) {
        multicore_fifo_push_blocking(message.data[i]);
    }
    messages_.pop_front();
}
} // namespace mini_lcd
