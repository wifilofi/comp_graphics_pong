#pragma once

#include "../../Engine/Game/GameContainer.h"

namespace Pong {

class PongGameContainer : public GameContainer
{
public:
    void Setup(Game* pGame) override;
};

}
