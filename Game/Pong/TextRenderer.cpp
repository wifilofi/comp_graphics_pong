#include "TextRenderer.h"
#include "../../Engine/Render/Pipeline.h"

using namespace Pong;

void TextRenderer::Compose(float2 position, float fontSize, float4 color)
{
    position_ = position;
    fontSize_ = fontSize;
    color_    = color;
}

void TextRenderer::Compose(Engine::Render::Pipeline *pPipeline)
{
    pPipeline_    = pPipeline;
    commonStates_ = std::make_unique<DirectX::CommonStates>(pPipeline->GetDevice());
    spriteBatch_  = std::make_unique<DirectX::SpriteBatch>(pPipeline->GetDeviceContext());
    try
    {
        spriteFont_ = std::make_unique<DirectX::SpriteFont>(pPipeline->GetDevice(), L"Fonts/teko.spritefont");
        scale_ = fontSize_ / spriteFont_->GetLineSpacing();
    }
    catch (...) {}
}

void TextRenderer::Render(float delta)
{
    if (!spriteFont_)
        return;

    const DirectX::XMVECTOR measured = spriteFont_->MeasureString(text_.c_str());
    const float textWidth  = DirectX::XMVectorGetX(measured) * scale_;
    const float textHeight = DirectX::XMVectorGetY(measured) * scale_;

    const DXViewport& vp = pPipeline_->GetViewport();
    DirectX::XMFLOAT2 pos{};
    pos.x = std::floor(vp.TopLeftX + position_.x * vp.Width  - textWidth  * 0.5f);
    pos.y = std::floor(vp.TopLeftY + position_.y * vp.Height - textHeight * 0.5f);

    const DirectX::XMVECTOR colorVec = DirectX::XMLoadFloat4(&color_);

    spriteBatch_->Begin(DirectX::SpriteSortMode_Deferred, nullptr, commonStates_->LinearClamp());
    spriteFont_->DrawString(spriteBatch_.get(), text_.c_str(), pos, colorVec,
                            0.f, DirectX::XMFLOAT2(0.f, 0.f), scale_);
    spriteBatch_->End();
}

void TextRenderer::SetText(std::wstring text)
{
    text_ = std::move(text);
}
