#pragma once
#include "../../Engine/Render/Able.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <memory>
#include <string>

namespace Pong
{
    class TextRenderer : public Engine::Render::Able
    {
    public:
        void Compose(Engine::Render::Pipeline *pPipeline) override;

        void Render(float delta) override;

        void SetText(std::wstring text);

    private:
        std::unique_ptr<DirectX::SpriteBatch> spriteBatch_;
        std::unique_ptr<DirectX::SpriteFont> spriteFont_;
        std::wstring text_;
        Engine::Render::Pipeline *pPipeline_ = nullptr;
    };
}
