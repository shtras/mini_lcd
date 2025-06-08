#pragma once
#include <array>

namespace mini_lcd
{
template <typename T>
class CircleDeq
{
public:
    void push_back(T value)
    {
        data_[end_] = value;
        end_ = (end_ + 1) % kSize;
        if (end_ == start_) {
            start_ = (start_ + 1) % kSize; // Overwrite the oldest element
        }
    }
    T pop_front()
    {
        if (start_ == end_) {
            return T{};
        }
        T value = data_[start_];
        start_ = (start_ + 1) % kSize;
        return value;
    }
    T front() const
    {
        if (start_ == end_) {
            return T{};
        }
        return data_[start_];
    }
    bool empty() const
    {
        return start_ == end_;
    }
private:
    static constexpr int kSize = 128;
    std::array<T, kSize> data_;
    int start_ = 0;
    int end_ = 0;
};
} // namespace mini_lcd
