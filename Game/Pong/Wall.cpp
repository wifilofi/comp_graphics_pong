#include "Wall.h"

#include "Ball.h"

using namespace Pong;

void Wall::Compose(const float2& center, const float2& size)
{
    boundingBox_.Center = float3(center);
    boundingBox_.Extents = float3(size);
}

void Wall::Collided(CollideAble* pOther)
{
    if (dynamic_cast<Ball*>(pOther) == nullptr) return;
    CollidedEvent.Broadcast(true);
}
