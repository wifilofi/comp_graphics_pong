#pragma once
#include "../../Engine/Game/GameContainer.h"

namespace Katamari
{
    class KatamariGameContainer : public GameContainer
    {
    public:
        void Setup(Game* pGame) override;
    };
}
