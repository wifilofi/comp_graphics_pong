#include "SolarBody.h"

#define _USE_MATH_DEFINES
#include <math.h>

using namespace Solar;

void SolarBody::Construct(const Params& p, const SolarBody* parent)
{
    parent_           = parent;
    shape_            = p.shape;
    scale_            = p.scale;
    color_            = p.color;
    color2_           = p.color2;
    orbitRadius_      = p.orbitRadius;
    orbitSpeed_       = p.orbitSpeed;
    orbitInclination_ = p.orbitInclination;
    selfRotSpeed_     = p.selfRotSpeed;
    orbitAngle_       = p.orbitAngleOffset;
}

void SolarBody::FixedUpdate()
{
    orbitAngle_   += orbitSpeed_;
    selfRotAngle_ += selfRotSpeed_;

    const float3 parentPos = parent_ ? parent_->GetPosition() : float3(0, 0, 0);

    const float localX = orbitRadius_ * cosf(orbitAngle_);
    const float localZ = orbitRadius_ * sinf(orbitAngle_);
    const float localY = localX * sinf(orbitInclination_);
    const float adjX   = localX * cosf(orbitInclination_);

    position_ = parentPos + float3(adjX, localY, localZ);
}

Basic::Components::Rendering3D::ObjectData SolarBody::GetInstanceData() const
{
    Basic::Components::Rendering3D::ObjectData data;
    data.model  = ComputeModelMatrix().Transpose();
    data.color  = color_;
    data.color2 = color2_;
    return data;
}

float4x4 SolarBody::ComputeModelMatrix() const
{
    const float4x4 scale = float4x4::CreateScale(scale_);
    const float4x4 rot   = float4x4::CreateRotationY(selfRotAngle_);
    const float4x4 trans = float4x4::CreateTranslation(position_);
    return scale * rot * trans;
}
