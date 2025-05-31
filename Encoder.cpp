#include "Encoder.h"
#include <map>
#include <iostream>

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
        button_ = std::make_shared<Button>(pinButton, onPress);
    }

    encoder_map[pinA_] = this;
    encoder_map[pinB_] = this;

    gpio_set_irq_enabled_with_callback(
        pinA_, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &Encoder::irq_callback);
    gpio_set_irq_enabled_with_callback(
        pinB_, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &Encoder::irq_callback);
}

void Encoder::process()
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
    if (encoder_map.find(gpio) != encoder_map.end()) {
        encoder_map.at(gpio)->callback(gpio, event);
    }
}

void Encoder::callback(uint gpio, uint32_t event)
{
    if (event != GPIO_IRQ_EDGE_RISE && event != GPIO_IRQ_EDGE_FALL) {
        return;
    }
    if (event == GPIO_IRQ_EDGE_RISE) {
        checkingDirection_ = false;
    }
    if (event == GPIO_IRQ_EDGE_FALL) {
        if (checkingDirection_) {
            if (gpio == pinA_) {
                directions_.push_back(Direction::Left);
            } else {
                directions_.push_back(Direction::Right);
            }
        }
        checkingDirection_ = true;
    }
}

} // namespace mini_lcd
