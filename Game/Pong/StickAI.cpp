#include "StickAI.h"

using namespace Pong;

void StickAI::Construct(const float2& center, const float2& size, Side side, float speed, Ball* pBall)
{
    Stick::Construct(center, size, side, speed);
    pBall_ = pBall;
}

void StickAI::FixedUpdate()
{
    const float limit = 1.0f - static_cast<float>(boundingBox_.Extents.y);
    boundingBox_.Center.y = Clamp(boundingBox_.Center.y, limit, -limit);
    velocity_ = float2();

    const auto difference = pBall_->GetBoundingBox().Center.y - boundingBox_.Center.y;
    if (difference > boundingBox_.Extents.y / 3)
        velocity_.y += speed_;
    else if (difference < -boundingBox_.Extents.y / 3)
        velocity_.y -= speed_;
}
