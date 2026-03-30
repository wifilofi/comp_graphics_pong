#include <windows.h>

#include "Engine/Game/Game.h"
#include "Engine/Render/BackgroundRenderer.h"
#include "Game/Pong/Ball.h"
#include "Game/Pong/Counter.h"
#include "Engine/Render/TextRenderer.h"
#include "Game/Pong/StickAI.h"
#include "Game/Pong/StickPlayer.h"
#include "Game/Pong/Wall.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
//
    auto *game = Game::Instance();
    auto *ball = new Pong::Ball();
    ball->Construct(float2(0, 0), float2(0.05f * 2 / 3, 0.05f), 1, 0.02f);

    auto *stick1 = new Pong::StickPlayer();
    stick1->Construct(float2(-0.9f, 0), float2(0.02f, 0.3f), Pong::Side::Left, 1, game->GetInputDevice());
    auto *stick2 = new Pong::StickAI();
    stick2->Construct(float2(0.9f, 0), float2(0.02f, 0.3f), Pong::Side::Right, 1, ball);

    auto *wallUp = new Pong::Wall();
    wallUp->Construct(float2(0, 2), float2(2, 1));
    auto *wallDown = new Pong::Wall();
    wallDown->Construct(float2(0, -2), float2(2, 1));
    auto *wallLeft = new Pong::Wall();
    wallLeft->Construct(float2(-2.1f, 0), float2(1, 2));
    auto *wallRight = new Pong::Wall();
    wallRight->Construct(float2(2.1f, 0), float2(1, 2));

    auto *counter = new Pong::Counter();
    counter->Construct(wallLeft, wallRight);

    auto *textRenderer = new Engine::Render::TextRenderer();

    textRenderer->Construct(float2(0.5f, 0.1f), 80.f, float4(1.f, 1.f, 1.f, 1.f));
    counter->ScoreChangedEvent.AddRaw(textRenderer, &Engine::Render::TextRenderer::SetText);

    auto *background = new Engine::Render::BackgroundRenderer();
    background->Construct(float2(0.f, 0.f), float2(1.f, 1.f));

    game->Construct(L"Game", 0.01f);

    game->GetRenderPipeline()->Add(background);
    game->GetRenderPipeline()->Add(counter);
    game->GetRenderPipeline()->Add(textRenderer);
    game->GetRenderPipeline()->Add(stick1);
    game->GetRenderPipeline()->Add(stick2);
    game->GetRenderPipeline()->Add(ball);
    game->GetRenderPipeline()->Add(counter);

    game->GetFixedUpdate()->Add(stick1);
    game->GetFixedUpdate()->Add(stick2);
    game->GetFixedUpdate()->Add(ball);

    game->GetPhysicsCollide()->Add(stick1);
    game->GetPhysicsCollide()->Add(stick2);
    game->GetPhysicsCollide()->Add(ball);
    game->GetPhysicsCollide()->Add(wallUp);
    game->GetPhysicsCollide()->Add(wallDown);
    game->GetPhysicsCollide()->Add(wallLeft);
    game->GetPhysicsCollide()->Add(wallRight);

    game->GetPhysicsMove()->Add(stick1);
    game->GetPhysicsMove()->Add(stick2);
    game->GetPhysicsMove()->Add(ball);

    game->Run();
    game->Destroy();
}
