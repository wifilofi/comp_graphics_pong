#include <windows.h>

#include "Engine/Game/Game.h"
#include "Game/Solar/SolarGameContainer.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    auto *game = Game::Instance();
    game->Construct(L"Solar System", 0.01f);

    auto *container = new Solar::SolarGameContainer();
    container->Setup(game);

    game->Run();
    game->Destroy();
}
