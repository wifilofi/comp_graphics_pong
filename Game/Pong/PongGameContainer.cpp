#include "PongGameContainer.h"

#include "../../Engine/Game/Game.h"
#include "../../Engine/Render/BackgroundRenderer.h"
#include "../../Engine/Render/TextRenderer.h"
#include "../../Game/Pong/Ball.h"
#include "../../Game/Pong/Counter.h"
#include "../../Game/Pong/StickAI.h"
#include "../../Game/Pong/StickPlayer.h"
#include "../../Game/Pong/Wall.h"

namespace Pong {

void PongGameContainer::Setup(Game* pGame)
{
    constexpr float4 gameColor = float4(0.561f, 0.984f, 0.91f, 1.0f);

    auto *ball = new Ball();
    ball->Construct(float2(0, 0), float2(0.05f * 2 / 3, 0.05f), 1, 0.02f, gameColor);

    auto *stick1 = new StickPlayer();
    stick1->Construct(float2(-0.9f, 0), float2(0.02f, 0.3f), Side::Left, 1, pGame->GetInputDevice(), gameColor);
    auto *stick2 = new StickAI();
    stick2->Construct(float2(0.9f, 0), float2(0.02f, 0.3f), Side::Right, 1, ball, gameColor);

    auto *wallUp = new Wall();
    wallUp->Construct(float2(0, 2), float2(2, 1));
    auto *wallDown = new Wall();
    wallDown->Construct(float2(0, -2), float2(2, 1));
    auto *wallLeft = new Wall();
    wallLeft->Construct(float2(-2.1f, 0), float2(1, 2));
    auto *wallRight = new Wall();
    wallRight->Construct(float2(2.1f, 0), float2(1, 2));

    auto *counter = new Counter();
    counter->Construct(wallLeft, wallRight);

    auto *textRenderer = new Engine::Render::TextRenderer();
    textRenderer->Construct(float2(0.5f, 0.1f), 80.f, gameColor);
    counter->ScoreChangedEvent.AddRaw(textRenderer, &Engine::Render::TextRenderer::SetText);

    auto *background = new Engine::Render::BackgroundRenderer();
    background->Construct(float2(0.f, 0.f), float2(1.f, 1.f));

    pGame->GetRenderPipeline()->Add(background);
    pGame->GetRenderPipeline()->Add(counter);
    pGame->GetRenderPipeline()->Add(textRenderer);
    pGame->GetRenderPipeline()->Add(stick1);
    pGame->GetRenderPipeline()->Add(stick2);
    pGame->GetRenderPipeline()->Add(ball);
    pGame->GetRenderPipeline()->Add(counter);

    pGame->GetFixedUpdate()->Add(stick1);
    pGame->GetFixedUpdate()->Add(stick2);
    pGame->GetFixedUpdate()->Add(ball);

    pGame->GetPhysicsCollide()->Add(stick1);
    pGame->GetPhysicsCollide()->Add(stick2);
    pGame->GetPhysicsCollide()->Add(ball);
    pGame->GetPhysicsCollide()->Add(wallUp);
    pGame->GetPhysicsCollide()->Add(wallDown);
    pGame->GetPhysicsCollide()->Add(wallLeft);
    pGame->GetPhysicsCollide()->Add(wallRight);

    pGame->GetPhysicsMove()->Add(stick1);
    pGame->GetPhysicsMove()->Add(stick2);
    pGame->GetPhysicsMove()->Add(ball);
}

} // namespace Pong