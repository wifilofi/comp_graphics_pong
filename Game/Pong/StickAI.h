#pragma once

#include "Ball.h"
#include "Stick.h"

namespace Pong
{
    class StickAI final : public Stick
    {
    public:
        void Construct(const float2& center, const float2& size, Side side, float speed, Ball* pBall, float4 color = float4(1, 1, 1, 1));

        void FixedUpdate() override;

    private:
        Ball* pBall_ = nullptr;
    };
}
