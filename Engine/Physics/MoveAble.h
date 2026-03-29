#pragma once

#include "../Lib/Types.h"

namespace Engine
{
    namespace Physics
    {
        class Move;

        class MoveAble
        {
        public:
            virtual ~MoveAble() = default;

            virtual void Compose(Move* pMove)
            {
            }

            virtual DXBox& GetBoundingBox() = 0;
            virtual float2& GetVelocity() = 0;
        };
    }
}
