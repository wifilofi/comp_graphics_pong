#include <windows.h>

#include "Engine/Game/Game.h"
#include "Game/Solar/SolarGameContainer.h"
#include "Game/Pong/PongGameContainer.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    auto *game = Game::Instance();
    game->Construct(L"Game of year", 0.01f);

    auto *container = new Solar::SolarGameContainer();
    //auto *container = new Pong::PongGameContainer();

    container->Setup(game);
    game->Run();
    game->Destroy();
}
