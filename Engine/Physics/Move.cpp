#include "Move.h"

using namespace Engine::Physics;

void Move::Compose(Update::Fixed* pFixed)
{
    pFixed_ = pFixed;
}

void Move::FixedUpdate()
{
    const auto delta = pFixed_->GetDelta();
    for (auto* pMoveAble : moveAbles_)
        pMoveAble->GetBoundingBox().Center = pMoveAble->GetBoundingBox().Center
            + float3(pMoveAble->GetVelocity()) * delta;
}

void Move::Add(MoveAble* pMoveAble)
{
    moveAbles_.push_back(pMoveAble);
    pMoveAble->Compose(this);
}
