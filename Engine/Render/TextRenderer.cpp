#include "TextRenderer.h"
#include "Pipeline.h"

using namespace Engine::Render;

void TextRenderer::Construct(float2 position, float fontSize, float4 color)
{
    position_ = position;
    fontSize_ = fontSize;
    color_    = color;
}

void TextRenderer::Construct(Engine::Render::Pipeline *pPipeline)
{
    pPipeline_    = pPipeline;
    commonStates_ = std::make_unique<DirectX::CommonStates>(pPipeline->GetDevice());
    spriteBatch_  = std::make_unique<DirectX::SpriteBatch>(pPipeline->GetDeviceContext());
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
    const float textWidth  = DirectX::XMVectorGetX(measured);
    const float textHeight = DirectX::XMVectorGetY(measured);

    const DXViewport& vp = pPipeline_->GetViewport();
    DirectX::XMFLOAT2 pos{};
    pos.x = std::floor(vp.TopLeftX + position_.x * vp.Width  - textWidth  * 0.5f);
    pos.y = std::floor(vp.TopLeftY + position_.y * vp.Height - textHeight * 0.5f);

    const DirectX::XMVECTOR colorVec = DirectX::XMLoadFloat4(&color_);

    spriteBatch_->Begin(DirectX::SpriteSortMode_Deferred, nullptr, commonStates_->PointClamp());
    spriteFont_->DrawString(spriteBatch_.get(), text_.c_str(), pos, colorVec);
    spriteBatch_->End();
}

void TextRenderer::SetText(std::wstring text)
{
    text_ = std::move(text);
}
