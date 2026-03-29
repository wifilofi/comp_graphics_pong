#include "StickPlayer.h"

using namespace Pong;

void StickPlayer::Compose(const float2 &center, const float2 &size, Side side, float speed,
                          Engine::Input::Device *pInputDevice)
{
    Stick::Compose(center, size, side, speed);
    pInputDevice_ = pInputDevice;
}

void StickPlayer::FixedUpdate()
{
    boundingBox_.Center.y = Clamp(boundingBox_.Center.y, 0.8f, -0.8f);
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
