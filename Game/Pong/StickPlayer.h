#pragma once

#include "Stick.h"
#include "../../Engine/Lib/Types.h"
#include "../../Engine/Input/Device.h"

namespace Pong
{
    class StickPlayer final : public Stick
    {
    public:
        void Construct(const float2& center, const float2& size, Side side, float speed,
            Engine::Input::Device* pInputDevice, float4 color = float4(1, 1, 1, 1));

        void FixedUpdate() override;

    private:
        Engine::Input::Device* pInputDevice_{};
    };
}
