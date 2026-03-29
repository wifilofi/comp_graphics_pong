#include "Game.h"
#include "../Input/Keyboard.h"

using namespace Engine;

Game *Game::Instance()
{
    static auto *instance = new Game();
    return instance;
}

void Game::Compose(const char16 *pName, float fixedDelta) const
{
    constexpr Point size = {1200, 800};
    window_->Compose(pName, size);
    inputDevice_->Compose(window_->GetHandlerWindow());
    pipeline_->Compose(window_->GetHandlerWindow(), size);
    fixedUpdate_->Compose(fixedDelta);
    fixedUpdate_->Add(physicsMove_);
    fixedUpdate_->Add(physicsCollide_);
}

void Game::Run()
{
    while (!isFinished_)
    {
        Input();
        fixedUpdate_->Update();
        Update();
        pipeline_->Render(static_cast<float>(fixedUpdate_->GetTimer().GetDelta()));
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
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (msg.message == WM_QUIT) isFinished_ = true;
    if (inputDevice_->IsKeyDown(Input::Keyboard::Keys::Escape)) isFinished_ = true;
}

void Game::Update()
{
}
