#pragma once

#include "../../Engine/Lib/Types.h"

namespace Pong
{
    struct AdditionData
    {
        float4 Offset;
        float4 Color;

        AdditionData() = default;

        AdditionData(const float4 &position, const float4 &color) : Offset(position), Color(color)
        {
        }
    };
}
