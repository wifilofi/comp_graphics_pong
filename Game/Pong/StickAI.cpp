#include "StickAI.h"

using namespace Pong;

void StickAI::Compose(const float2& center, const float2& size, Side side, float speed, Ball* pBall)
{
    Stick::Compose(center, size, side, speed);
    pBall_ = pBall;
}

void StickAI::FixedUpdate()
{
    boundingBox_.Center.y = Clamp(boundingBox_.Center.y, 0.8f, -0.8f);
    velocity_ = float2();

    const auto difference = pBall_->GetBoundingBox().Center.y - boundingBox_.Center.y;
    if (difference > boundingBox_.Extents.y / 3)
        velocity_.y += speed_;
    else if (difference < -boundingBox_.Extents.y / 3)
        velocity_.y -= speed_;
}
