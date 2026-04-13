#pragma once

#include <vector>

#include "FixedAble.h"
#include "../Lib/TimeAccumulator.h"

namespace Engine
{
    namespace Update
    {
        class FixedAble;

        class Fixed final
        {
        public:
            void Construct(float delta);
            void Update();

            void Add(FixedAble* pFixedUpdateAble);

            Lib::TimeAccumulator& GetTimer() { return timer_; }
            float GetDelta() const { return delta_; }
            float GetAlpha() const { return static_cast<float>(timer_.GetAccumulated() / delta_); }

        private:
            Lib::TimeAccumulator timer_;
            std::vector<FixedAble*> fixedUpdateAbles_{};
            float delta_ = 0;
        };
    }
}
