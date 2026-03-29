#pragma once

#include "../../Engine/Lib/Types.h"

namespace Pong
{
    struct AdditionData
    {
        float4 Offset;
        float4 Color;
        float4 Size;

        AdditionData() = default;

        AdditionData(const float4 &position, const float4 &color, const float4 &size) : Offset(position), Color(color), Size(size)
        {
        }
    };
}
