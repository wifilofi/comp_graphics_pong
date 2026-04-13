#include "SolarGameContainer.h"

#include "../../Engine/Game/Game.h"
#include "SolarSystem.h"
#include "../../Engine/Render/BackgroundRenderer.h"

using namespace Solar;

void SolarGameContainer::Setup(Game* pGame)
{
    pGame->GetRenderPipeline()->SetBackgroundColor({0.f, 0.f, 0.f, 1.f});

    auto* system = new SolarSystem();

    system->SetInputDevice(pGame->GetInputDevice());

    pGame->GetRenderPipeline()->Add(system);
    pGame->GetFixedUpdate()->Add(system);
}
