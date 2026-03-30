#pragma once
#include "../../Engine/Render/Able.h"
#include "../../Engine/Lib/Types.h"
#include <CommonStates.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <memory>
#include <string>

namespace Pong
{
    class TextRenderer : public Engine::Render::Able
    {
    public:
        void Compose(float2 position, float fontSize, float4 color);

        void Compose(Engine::Render::Pipeline *pPipeline) override;

        void Render(float delta) override;

        void SetText(std::wstring text);

    private:
        std::unique_ptr<DirectX::CommonStates> commonStates_;
        std::unique_ptr<DirectX::SpriteBatch> spriteBatch_;
        std::unique_ptr<DirectX::SpriteFont> spriteFont_;
        std::wstring text_;
        float2 position_  = { 0.5f, 0.05f };
        float  fontSize_  = 32.f;
        float4 color_     = { 1.f, 1.f, 1.f, 1.f };
        float  scale_     = 1.f;
        Engine::Render::Pipeline *pPipeline_ = nullptr;
    };
}
