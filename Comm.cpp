#include "Comm.h"

#include <pico/stdlib.h>
#include <pico/multicore.h>

#include <plog/Log.h>

#include <iostream>
#include <cstring>

namespace mini_lcd
{
const std::map<Message::Type, int> Message::Size = {{Message::Type::Measurements, 21}};

Message* Receiver::process()
{
    if (!multicore_fifo_rvalid()) {
        return nullptr;
    }
    switch (state_) {
        case State::Idle: {
            uint32_t val = multicore_fifo_pop_blocking();
            Message::Type type = static_cast<Message::Type>(val);
            // PLOG_VERBOSE << "Now receiving " << val;
            message_.type = type;
            if (Message::Size.count(type) == 0) {
                PLOG_ERROR << "Bad message type: " << val;
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
            // PLOG_VERBOSE << "Received " << received_ << " out of " << toReceive_;
            if (received_ >= toReceive_) {
                // PLOG_VERBOSE << "Final";
                state_ = State::Idle;
                received_ = 0;
                return &message_;
            }
            break;
        }
    }
    return nullptr;
}
} // namespace mini_lcd
