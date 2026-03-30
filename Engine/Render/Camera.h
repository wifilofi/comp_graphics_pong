#pragma once

#include "../Lib/Types.h"

namespace Engine::Render
{
    class Camera
    {
    public:
        virtual ~Camera() = default;

        virtual float4x4 GetView() const = 0;
        virtual float4x4 GetProjection() const = 0;

        // Called each fixed update tick
        virtual void FixedUpdate() {}
    };
}
