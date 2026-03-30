#include "TextRenderer.h"
#include "../../Engine/Render/Pipeline.h"
#include <DirectXColors.h>

using namespace Pong;

void TextRenderer::Compose(Engine::Render::Pipeline *pPipeline)
{
    pPipeline_ = pPipeline;
    spriteBatch_ = std::make_unique<DirectX::SpriteBatch>(pPipeline->GetDeviceContext());
    try
    {
        spriteFont_ = std::make_unique<DirectX::SpriteFont>(pPipeline->GetDevice(), L"Fonts/teko.spritefont");
    }
    catch (...) {}
}

void TextRenderer::Render(float delta)
{
    if (!spriteFont_)
        return;

    const DirectX::XMVECTOR measured = spriteFont_->MeasureString(text_.c_str());
    const float textWidth = DirectX::XMVectorGetX(measured);

    const DXViewport& vp = pPipeline_->GetViewport();
    const DirectX::XMFLOAT2 pos = {
        vp.TopLeftX + (vp.Width - textWidth) * 0.5f,
        vp.TopLeftY + 20.f
    };

    spriteBatch_->Begin();
    spriteFont_->DrawString(spriteBatch_.get(), text_.c_str(), pos, DirectX::Colors::White);
    spriteBatch_->End();
}

void TextRenderer::SetText(std::wstring text)
{
    text_ = std::move(text);
}
