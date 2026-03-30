#include "TimeAccumulator.h"

using namespace Lib;

void TimeAccumulator::Construct()
{
    current_ = std::chrono::steady_clock::now();
}

void TimeAccumulator::Update()
{
    const auto current = std::chrono::steady_clock::now();
    delta_ = std::chrono::duration<double>(current - current_).count();
    current_ = current;
    accumulated_ += delta_;
}
