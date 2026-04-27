#include "KatamariGameContainer.h"
#include "../../Engine/Game/Game.h"
#include "KatamariWorld.h"

using namespace Katamari;

void KatamariGameContainer::Setup(Game* pGame)
{
    pGame->GetRenderPipeline()->SetBackgroundColor({0.45f, 0.65f, 0.9f, 1.f});

    auto* world = new KatamariWorld();
    world->SetInputDevice(pGame->GetInputDevice());

    pGame->GetRenderPipeline()->Add(world);
    pGame->GetFixedUpdate()->Add(world);
}
