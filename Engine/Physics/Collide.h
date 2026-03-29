#pragma once

#include <vector>

#include "CollideAble.h"
#include "../Update/FixedAble.h"

namespace Engine
{
    namespace Physics
    {
        class CollideAble;

        class Collide final : public Update::FixedAble
        {
        public:
            void FixedUpdate() override;

            void Add(CollideAble* pCollideAble);

        private:
            static void DetectCollision(CollideAble* pCollideAble1, CollideAble* pCollideAble2);

            std::vector<CollideAble*> physicsAbles_{};
        };
    }
}
