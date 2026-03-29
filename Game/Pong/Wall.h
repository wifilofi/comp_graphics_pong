#pragma once

#include "../../Engine/Physics/CollideAble.h"
#include "../../Engine/Lib/Delegates.h"

namespace Pong
{
    class Wall final : public Engine::Physics::CollideAble
    {
    public:
        void Compose(const float2 &center, const float2 &size);

        void Collided(CollideAble *pOther) override;

        DXBox &GetBoundingBox() override { return boundingBox_; }

        Lib::MulticastDelegate<bool> CollidedEvent;

    private:
        DXBox boundingBox_;
    };
}
