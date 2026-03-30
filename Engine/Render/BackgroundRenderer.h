#pragma once
#include "../Basic/Shapes/Square.h"
#include "../Lib/Types.h"

namespace Engine::Render
{
    class BackgroundRenderer : public Basic::Shapes::Square
    {
    public:
        void Compose(const float2& center, const float2& size);
        void Compose(Engine::Render::Pipeline* pPipeline) override;
        void Render(float delta) override;

    private:
        DXBuffer* pAdditionDataBuffer_ = nullptr;
        float2 extents_;
    };
}
