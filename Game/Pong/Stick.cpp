#include "Stick.h"
#include "../../Engine/Render/ShaderData.h"
#include "directxmath.h"

using namespace Pong;

void Stick::Compose(const float2& center, const float2& size, Side side, float speed)
{
    sprite_.Compose(center, size);
    hand_ = side;
    speed_ = speed;
    boundingBox_ = DXBox(float3(center), float3(size));
    startPosition_ = float3(center);
}

void Stick::Compose(Engine::Render::Pipeline* pPipeline)
{
    sprite_.Compose(pPipeline);

    D3D11_BUFFER_DESC bufferDescriptor;
    bufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
    bufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDescriptor.MiscFlags = 0;
    bufferDescriptor.StructureByteStride = 0;
    bufferDescriptor.ByteWidth = sizeof(Engine::Render::ShaderData);

    sprite_.GetPipeline()->GetDevice()->CreateBuffer(&bufferDescriptor, nullptr, &pAdditionDataBuffer_);
}

void Stick::Render(float delta)
{
    const float3 offset = boundingBox_.Center - startPosition_;
    const float4 size(boundingBox_.Extents.x, boundingBox_.Extents.y, 0, 0);
    const Engine::Render::ShaderData additionData{float4(offset.x, offset.y, offset.z, 1), float4(1), size};
    D3D11_MAPPED_SUBRESOURCE subresource = {};
    sprite_.GetPipeline()->GetDeviceContext()->Map(
        pAdditionDataBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
    auto* pData = static_cast<float*>(subresource.pData);
    memcpy(pData, &additionData, sizeof(Engine::Render::ShaderData));
    sprite_.GetPipeline()->GetDeviceContext()->Unmap(pAdditionDataBuffer_, 0);
    sprite_.GetPipeline()->GetDeviceContext()->VSSetConstantBuffers(0, 1, &pAdditionDataBuffer_);
    sprite_.GetPipeline()->GetDeviceContext()->PSSetConstantBuffers(0, 1, &pAdditionDataBuffer_);

    sprite_.Render(delta);
}

float Stick::Clamp(float x, float upper, float down)
{
    //TODO: use other min max?
    return min(upper, max(x, down));
}
