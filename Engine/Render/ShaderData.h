#pragma once

#include "../Lib/Types.h"

namespace Engine::Render
{
    struct ShaderData
    {
        float4 Offset;
        float4 Color;
        float4 Size;

        ShaderData() = default;

        ShaderData(const float4 &position, const float4 &color, const float4 &size) : Offset(position), Color(color), Size(size)
        {
        }
    };
}
