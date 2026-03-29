#include "Counter.h"
#include "../../Engine/Render/Pipeline.h"
#include <string>

using namespace Pong;

void Counter::Compose(Wall *pLeft, Wall *pRight)
{
    pLeft->CollidedEvent.AddRaw(this, &Counter::OnCollidedEvent, 1);
    pRight->CollidedEvent.AddRaw(this, &Counter::OnCollidedEvent, 2);
}

void Counter::Compose(Engine::Render::Pipeline *pPipeline)
{
    pPipeline_ = pPipeline;
}

void Counter::Render(float delta)
{
}

void Counter::OnCollidedEvent(bool _, int32 n)
{
    if (n == 1) points1++;
    else points2++;

    if (pPipeline_)
    {
        std::wstring title = L"Pong - " + std::to_wstring(points1) + L" : " + std::to_wstring(points2);
        SetWindowText(pPipeline_->GetWindow(), title.c_str());
    }
}
