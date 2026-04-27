#include <windows.h>

#include "Engine/Game/Game.h"
#include "Game/Katamari/KatamariGameContainer.h"
#include "Game/Solar/SolarGameContainer.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    auto *game = Game::Instance();
    game->Construct(L"Game of year", 0.01f);

    //auto *container = new Solar::SolarGameContainer();
    //auto *container = new Pong::PongGameContainer();
    auto *container = new Katamari::KatamariGameContainer();

    container->Setup(game);
    game->Run();
    game->Destroy();
}
