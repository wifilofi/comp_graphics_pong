#include "SolarGameContainer.h"

#include "../../Engine/Game/Game.h"
#include "SolarSystem.h"

using namespace Solar;

void SolarGameContainer::Setup(Game* pGame)
{
    auto* system = new SolarSystem();
    system->SetInputDevice(pGame->GetInputDevice());

    pGame->GetRenderPipeline()->Add(system);
    pGame->GetFixedUpdate()->Add(system);
}
