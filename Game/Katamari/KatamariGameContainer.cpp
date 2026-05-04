#include "KatamariGameContainer.h"
#include "../../Engine/Game/Game.h"
#include "KatamariWorld.h"

using namespace Katamari;

static const char kGammaCorrection[] = R"hlsl(
Texture2D    sceneTex : register(t0);
SamplerState smp      : register(s0);
cbuffer FrameData : register(b1) { float time; float _pad; float2 resolution; };
struct PS_IN { float4 pos : SV_POSITION; float2 uv : TEXCOORD0; };
float4 PSMain(PS_IN i) : SV_Target {
    float3 col = sceneTex.Sample(smp, i.uv).rgb;
    return float4(pow(max(col, 0.0001), 1.0 / 2.2), 1.0);
}
)hlsl";

void KatamariGameContainer::Setup(Game* pGame)
{
    pGame->GetRenderPipeline()->SetBackgroundColor({0.45f, 0.65f, 0.9f, 1.f});

    auto* world = new KatamariWorld();
    world->SetInputDevice(pGame->GetInputDevice());

    pGame->GetRenderPipeline()->Add(world);
    pGame->GetFixedUpdate()->Add(world);

    pGame->GetRenderPipeline()->AddPostProcess(kGammaCorrection);
}
