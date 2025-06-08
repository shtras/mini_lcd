#include "Encoder.h"
#include "Utils/Logger.h"

#include <map>
#include <iostream>

#include <hardware/sync.h>

static std::map<uint, mini_lcd::Encoder*> encoder_map;

namespace mini_lcd
{
Encoder::Encoder(uint pinA, uint pinB, uint pinButton, std::function<void(void)> onPress)
    : pinA_(pinA)
    , pinB_(pinB)
{
    gpio_set_dir(pinA_, GPIO_IN);
    gpio_set_dir(pinB_, GPIO_IN);
    gpio_pull_up(pinA_);
    gpio_pull_up(pinB_);
    if (onPress) {
        button_ = std::make_shared<Button>(pinButton, nullptr, onPress);
    }

    encoder_map[pinA_] = this;
    encoder_map[pinB_] = this;

    gpio_set_irq_enabled_with_callback(pinA_, GPIO_IRQ_EDGE_FALL, true, &Encoder::irq_callback);
    gpio_set_irq_enabled_with_callback(pinB_, GPIO_IRQ_EDGE_FALL, true, &Encoder::irq_callback);
}

void Encoder::SetOnLeft(std::function<void(void)> onLeft)
{
    onLeft_ = onLeft;
}

void Encoder::SetOnRight(std::function<void(void)> onRight)
{
    onRight_ = onRight;
}

void Encoder::SetOnPress(std::function<void(void)> onPress)
{
    if (button_) {
        button_->SetOnDown(onPress);
    } else {
        button_ = std::make_shared<Button>(pinA_, nullptr, onPress);
    }
}

void Encoder::Process()
{
    while (!directions_.empty()) {
        if (directions_.front() == Direction::Left) {
            if (onLeft_) {
                onLeft_();
            }
        } else {
            if (onRight_) {
                onRight_();
            }
        }
        directions_.pop_front();
    }
    if (button_) {
        button_->Process();
    }
}

void Encoder::irq_callback(uint gpio, uint32_t event)
{
    auto status = save_and_disable_interrupts();
    if (encoder_map.find(gpio) != encoder_map.end()) {
        encoder_map.at(gpio)->callback(gpio, event);
    }
    restore_interrupts(status);
}

void Encoder::callback(uint gpio, uint32_t event)
{
    /*
    https://old.reddit.com/r/raspberrypipico/comments/pacarb/sharing_some_c_code_to_read_a_rotary_encoder/
CW rotation 
______       ______
      F1____|           Phase A
_________       ______
        F2_____|        Phase B 
Notice that Phase A falling edge (F1 in the diagram) occurs before Phase B (F2) for CW rotation



CCW Rotation
_________      ______
        F2____|         Phase A 
______       ______
      F1____|           Phase B
    */

    auto pins = gpio_get_all();
    auto pinA = (pins & (1u << pinA_)) != 0;
    auto pinB = (pins & (1u << pinB_)) != 0;

    if (gpio == pinA_) {
        if ((!cwFall_) && (!pinA && pinB)) {
            cwFall_ = 1;
        }

        if ((ccwFall_) && (!pinA && !pinB)) {
            cwFall_ = 0;
            ccwFall_ = 0;
            directions_.push_back(Direction::Right);
        }
    }
    if (gpio == pinB_) {
        if ((!ccwFall_) && (pinA && !pinB)) {
            ccwFall_ = 1;
        }

        if ((cwFall_) && (!pinA && !pinB)) {
            cwFall_ = 0;
            ccwFall_ = 0;
            directions_.push_back(Direction::Left);
        }
    }
}

} // namespace mini_lcd
