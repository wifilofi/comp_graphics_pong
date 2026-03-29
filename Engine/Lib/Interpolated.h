#pragma once

namespace Lib
{
    template<typename T>
    class Interpolated final
    {
    public:
        void Advance(const T& next) { prev_ = curr_; curr_ = next; }
        T Get(float alpha) const { return prev_ + (curr_ - prev_) * alpha; }
        const T& Current() const { return curr_; }

    private:
        T prev_{};
        T curr_{};
    };
}