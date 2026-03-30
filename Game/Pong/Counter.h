#pragma once
#include "Wall.h"
#include "../../Engine/Render/Renderer.h"
#include <string>

namespace Pong
{
    class Counter : public Engine::Render::Renderer
    {
    public:
        void Construct(Wall *pLeft, Wall *pRight);

        void Construct(Engine::Render::Pipeline *pPipeline) override;

        void Render(float delta) override;

        Lib::MulticastDelegate<std::wstring> ScoreChangedEvent;

    private:
        void OnCollidedEvent(bool _, int32 n);

        int32 points1 = 0;
        int32 points2 = 0;
        Engine::Render::Pipeline *pPipeline_ = nullptr;
    };
}
