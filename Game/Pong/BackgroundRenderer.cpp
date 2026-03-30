#include "BackgroundRenderer.h"
#include "../../Engine/Render/ShaderData.h"
#include "../../Engine/Basic/Components/Rendering.h"
#include "../../Engine/Render/Pipeline.h"

using namespace Pong;

void BackgroundRenderer::Compose(const float2& center, const float2& size)
{
    Square::Compose(center, size);
    extents_ = size;
}

void BackgroundRenderer::Compose(Engine::Render::Pipeline* pPipeline)
{
    Basic::Components::Rendering::Compose(pPipeline);
    CreateVertexShader(L"././Shaders/shader.hlsl", nullptr, nullptr);
    CreatePixelShader(L"././Shaders/shader.hlsl", nullptr, nullptr);
    CreateLayout();
    CreateVertexBuffer(4 * 2);
    CreateIndexBuffer(3 * 2);
    CreateRasterizerState();

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage          = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.ByteWidth      = sizeof(ShaderData);
    pPipeline_->GetDevice()->CreateBuffer(&bufferDesc, nullptr, &pAdditionDataBuffer_);
}

void BackgroundRenderer::Render(float delta)
{
    const ShaderData data{
        float4(0.f, 0.f, 0.f, 1.f),
        float4(1.f, 1.f, 1.f, 1.f),
        float4(extents_.x, extents_.y, 0.f, 0.f)
    };

    D3D11_MAPPED_SUBRESOURCE sub = {};
    pPipeline_->GetDeviceContext()->Map(pAdditionDataBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub);
    memcpy(sub.pData, &data, sizeof(ShaderData));
    pPipeline_->GetDeviceContext()->Unmap(pAdditionDataBuffer_, 0);
    pPipeline_->GetDeviceContext()->VSSetConstantBuffers(0, 1, &pAdditionDataBuffer_);
    pPipeline_->GetDeviceContext()->PSSetConstantBuffers(0, 1, &pAdditionDataBuffer_);

    Basic::Components::Rendering::Render(delta);
}
