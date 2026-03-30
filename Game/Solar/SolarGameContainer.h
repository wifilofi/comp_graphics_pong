#pragma once

#include "../../Engine/Game/GameContainer.h"

namespace Solar
{
    class SolarGameContainer : public GameContainer
    {
    public:
        void Setup(Game* pGame) override;
    };
}
