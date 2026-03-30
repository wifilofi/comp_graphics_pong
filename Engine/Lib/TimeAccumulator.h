#pragma once

#include <chrono>

namespace Lib
{
    class TimeAccumulator final
    {
    public:
        void Construct();
        void Update();

        double GetAccumulated() const { return accumulated_; }
        void SetAccumulated(double value) { accumulated_ = value; }
        double GetDelta() const { return delta_; }

    private:
        std::chrono::steady_clock::time_point current_;
        double accumulated_ = 0;
        double delta_ = 0;
    };
}
