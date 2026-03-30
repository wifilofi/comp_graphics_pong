#include "StickPlayer.h"

using namespace Pong;

void StickPlayer::Construct(const float2 &center, const float2 &size, Side side, float speed,
                          Engine::Input::Device *pInputDevice, float4 color)
{
    Stick::Construct(center, size, side, speed, color);
    pInputDevice_ = pInputDevice;
}

void StickPlayer::FixedUpdate()
{
    const float limit = 1.0f - static_cast<float>(boundingBox_.Extents.y);
    boundingBox_.Center.y = Clamp(boundingBox_.Center.y, limit, -limit);
    velocity_ = float2();

    if (hand_ == Side::Left)
    {
        if (pInputDevice_->IsKeyDown(Engine::Input::Keyboard::Keys::W))
            velocity_.y += speed_;
        if (pInputDevice_->IsKeyDown(Engine::Input::Keyboard::Keys::S))
            velocity_.y -= speed_;
        return;
    }

    if (pInputDevice_->IsKeyDown(Engine::Input::Keyboard::Keys::UpArrow))
        velocity_.y += speed_;
    if (pInputDevice_->IsKeyDown(Engine::Input::Keyboard::Keys::DownArrow))
        velocity_.y -= speed_;
}
