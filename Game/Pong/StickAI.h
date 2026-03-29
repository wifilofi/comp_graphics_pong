#pragma once

#include "Ball.h"
#include "Stick.h"

namespace Pong
{
    class StickAI final : public Stick
    {
    public:
        void Compose(const float2& center, const float2& size, Side side, float speed, Ball* pBall);

        void FixedUpdate() override;

    private:
        Ball* pBall_ = nullptr;
    };
}
