#include "Collide.h"

#include "../Update/Fixed.h"

using namespace Engine::Physics;

void Collide::FixedUpdate()
{
    for (auto i = 0; i < physicsAbles_.size(); i++)
        for (auto j = i + 1; j < physicsAbles_.size(); j++)
            DetectCollision(physicsAbles_[i], physicsAbles_[j]);
}

void Collide::Add(CollideAble* pCollideAble)
{
    physicsAbles_.push_back(pCollideAble);
    pCollideAble->Compose(this);
}

void Collide::DetectCollision(CollideAble* pCollideAble1, CollideAble* pCollideAble2)
{
    if (!pCollideAble1->GetBoundingBox().Intersects(pCollideAble2->GetBoundingBox())) return;
    pCollideAble1->Collided(pCollideAble2);
    pCollideAble2->Collided(pCollideAble1);
}
