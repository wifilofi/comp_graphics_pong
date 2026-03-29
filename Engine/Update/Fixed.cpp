#include "Fixed.h"

using namespace Engine::Update;

void Fixed::Compose(float delta)
{
    timer_.Compose();
    delta_ = delta;
}

void Fixed::Update()
{
    timer_.Update();
    while (timer_.GetAccumulated() > static_cast<double>(delta_))
    {
        for (auto* pFixedUpdateAble : fixedUpdateAbles_) pFixedUpdateAble->FixedUpdate();
        timer_.SetAccumulated(timer_.GetAccumulated() - static_cast<double>(delta_));
    }
}

void Fixed::Add(FixedAble* pFixedUpdateAble)
{
    fixedUpdateAbles_.push_back(pFixedUpdateAble);
    pFixedUpdateAble->Compose(this);
}
