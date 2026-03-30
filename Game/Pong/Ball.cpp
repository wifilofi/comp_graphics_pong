//#define NOMINMAX
#include "Ball.h"

#include <cmath>

#include "../../Engine/Render/ShaderData.h"
#include "Stick.h"
#include "../../Engine/Game/Game.h"
#include <algorithm>
#include "../../Engine/Render/ShaderData.h"
using namespace Pong;

void Ball::Construct(const float2 &center, const float2 &size, float startSpeed, float speedIncrease)
{
    sprite_.Construct(center, size);
    startSpeed_ = startSpeed;
    speedIncrease_ = speedIncrease;
    velocity_ = float2(startSpeed, 0);
    boundingBox_ = DXBox(float3(center), float3(size));
    startPosition_ = float3(center);
}

void Ball::Construct(Engine::Render::Pipeline *pPipeline)
{
    sprite_.Construct(pPipeline);

    D3D11_BUFFER_DESC bufferDescriptor;
    bufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
    bufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDescriptor.MiscFlags = 0;
    bufferDescriptor.StructureByteStride = 0;
    bufferDescriptor.ByteWidth = sizeof(Engine::Render::ShaderData);

    sprite_.GetPipeline()->GetDevice()->CreateBuffer(&bufferDescriptor, nullptr, &pAdditionDataBuffer_);
}

void Ball::FixedUpdate()
{
    position_.Advance(boundingBox_.Center);
}

void Ball::Render(float alpha)
{
    const float3 offset = position_.Get(alpha) - startPosition_;
    const float4 size(boundingBox_.Extents.x, boundingBox_.Extents.y, 0, 0);
    const Engine::Render::ShaderData additionData{float4(offset.x, offset.y, offset.z, 1), float4(1), size};
    D3D11_MAPPED_SUBRESOURCE subresource = {};
    sprite_.GetPipeline()->GetDeviceContext()->Map(
        pAdditionDataBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
    auto *pData = static_cast<float *>(subresource.pData);
    memcpy(pData, &additionData, sizeof(Engine::Render::ShaderData));
    sprite_.GetPipeline()->GetDeviceContext()->Unmap(pAdditionDataBuffer_, 0);
    sprite_.GetPipeline()->GetDeviceContext()->VSSetConstantBuffers(0, 1, &pAdditionDataBuffer_);
    sprite_.GetPipeline()->GetDeviceContext()->PSSetConstantBuffers(0, 1, &pAdditionDataBuffer_);

    sprite_.Render(alpha);
}

void Ball::Collided(Engine::Physics::CollideAble *other)
{
    const auto *pStick = dynamic_cast<Stick *>(other);
    const auto &otherBoundingBox = other->GetBoundingBox();
    if (pStick == nullptr)
    {
        velocity_.y = -velocity_.y;
        if (abs(boundingBox_.Center.x) > 1)
        {
            boundingBox_.Center = float3();
            velocity_ = float2((velocity_.x < 0 ? startSpeed_ : -startSpeed_), 0);
        }
        if (abs(boundingBox_.Center.y) + boundingBox_.Extents.y > 1)
        {
            boundingBox_.Center.y = (1 - boundingBox_.Extents.y) * (boundingBox_.Center.y > 0 ? 1 : -1);
        }
        return;
    }

    velocity_.x += velocity_.x > 0 ? speedIncrease_ : -speedIncrease_;
    velocity_.x = Clamp(-velocity_.x, 3, -3);
    if (abs(boundingBox_.Center.x) + boundingBox_.Extents.x >
        abs(otherBoundingBox.Center.x) + otherBoundingBox.Extents.x)
    {
        boundingBox_.Center.x = (abs(otherBoundingBox.Center.x) - otherBoundingBox.Extents.x - boundingBox_.Extents.y)
                                * (boundingBox_.Center.x > 0 ? 1 : -1);
    }

    //TODO: min max
    const auto ratio = (boundingBox_.Center.y - otherBoundingBox.Center.y) / abs(otherBoundingBox.Extents.y);
    velocity_.y =
        static_cast<float>(min(floor(static_cast<double>(ratio) * 10), 9.0) / 10.0);
}

float Ball::Clamp(float x, float upper, float down)
{

    //TODO: min max
    return min(upper, max(x, down));
}
