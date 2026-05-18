#include "KatamariGameContainer.h"
#include "../../Engine/Game/Game.h"
#include "../../Engine/Render/PostProcess.h"
#include "KatamariWorld.h"

using namespace Katamari;

void KatamariGameContainer::Setup(Game* pGame)
{
    auto* pipeline = pGame->GetRenderPipeline();
    pipeline->SetBackgroundColor({0.45f, 0.65f, 0.9f, 1.f});

    auto* world = new KatamariWorld();
    world->SetInputDevice(pGame->GetInputDevice());
    pipeline->Add(world);
    pGame->GetFixedUpdate()->Add(world);

    auto* pp = new Engine::Render::PostProcess();
    const auto sz = pipeline->GetSize();
    pp->Construct(pipeline, sz.x, sz.y);


    pp->AddPass(L"././Shaders/GammaCorrection.hlsl");
    pp->AddPass(L"././Shaders/Posterize.hlsl");
    pipeline->SetPostProcess(pp);
}
