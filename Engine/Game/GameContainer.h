#pragma once

class Game;

class GameContainer
{
public:
    virtual ~GameContainer() = default;
    virtual void Setup(Game* pGame) = 0;
};