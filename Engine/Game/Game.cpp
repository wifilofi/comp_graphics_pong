#include "Game.h"
#include "../Input/Keyboard.h"

using namespace Engine;

Game *Game::Instance()
{

    static auto *instance = new Game();
    return instance;
}

void Game::Construct(const char16 *pName, float fixedDelta) const
{
    ///...
    constexpr Point size = {750, 500};
    window_->Construct(pName, size);
    inputDevice_->Compose(window_->GetHandlerWindow());
    pipeline_->Construct(window_->GetHandlerWindow(), size);
    fixedUpdate_->Construct(fixedDelta);
    fixedUpdate_->Add(physicsMove_);
    fixedUpdate_->Add(physicsCollide_);
}

void Game::Run()
{
    while (!isFinished_)
    {
        Input();
        if (isFinished_) break;
        fixedUpdate_->Update();
        Update();
        pipeline_->Render(fixedUpdate_->GetAlpha());
    }
}

void Game::Destroy() const
{
    pipeline_->Destroy();
    inputDevice_->Destroy();
    window_->Destroy();
    delete physicsMove_;
    delete physicsCollide_;
    delete fixedUpdate_;
    delete pipeline_;
    delete inputDevice_;
    delete window_;
}

void Game::Input()
{
    MSG msg;

    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT) { isFinished_ = true; return; }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
//
    if (inputDevice_->IsKeyDown(Input::Keyboard::Keys::Escape)) isFinished_ = true;
}

void Game::Update()
{
}
