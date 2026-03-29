#pragma once

#include "../Lib/Types.h"

namespace Engine
{
    namespace Physics
    {
        class Collide;

        class CollideAble
        {
        public:
            virtual ~CollideAble() = default;

            virtual void Compose(Collide* pCollide)
            {
            }

            virtual void Collided(CollideAble* pOther)
            {
            }

            virtual DXBox& GetBoundingBox() = 0;
        };
    }
}
