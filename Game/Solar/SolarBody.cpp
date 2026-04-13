#include "SolarBody.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "../../Engine/Basic/Shapes/Box.h"
#include "../../Engine/Basic/Shapes/LowPolySphere.h"
#include "../../Engine/Render/Pipeline.h"

using namespace Solar;

void SolarBody::Construct(Engine::Render::Pipeline* pPipeline, const Params& p,
                          const SolarBody* parent)
{
    parent_          = parent;
    scale_           = p.scale;
    color_           = p.color;
    color2_          = p.color2;
    orbitRadius_     = p.orbitRadius;
    orbitSpeed_      = p.orbitSpeed;
    orbitInclination_= p.orbitInclination;
    selfRotSpeed_    = p.selfRotSpeed;
    orbitAngle_      = p.orbitAngleOffset;

    if (p.shape == ShapeType::Sphere)
        rendering_.Construct(pPipeline, Basic::Shapes::LowPolySphere::Vertices(),
                                        Basic::Shapes::LowPolySphere::Indices(), p.shaderType);
    else
        rendering_.Construct(pPipeline, Basic::Shapes::Box::Vertices(),
                                        Basic::Shapes::Box::Indices(), p.shaderType);
}

void SolarBody::FixedUpdate()
{
    orbitAngle_  += orbitSpeed_;
    selfRotAngle_ += selfRotSpeed_;

    const float3 parentPos = parent_ ? parent_->GetPosition() : float3(0, 0, 0);

    // Orbit in the XZ plane, tilted by inclination around Z axis
    const float localX = orbitRadius_ * cosf(orbitAngle_);
    const float localZ = orbitRadius_ * sinf(orbitAngle_);
    const float localY = localX * sinf(orbitInclination_);
    const float adjX   = localX * cosf(orbitInclination_);

    position_ = parentPos + float3(adjX, localY, localZ);
}

void SolarBody::Render(float /*delta*/)
{
    rendering_.Draw(ComputeModelMatrix(), color_, color2_);
}

float4x4 SolarBody::ComputeModelMatrix() const
{
    const float4x4 scale  = float4x4::CreateScale(scale_);
    const float4x4 rot    = float4x4::CreateRotationY(selfRotAngle_);
    const float4x4 trans  = float4x4::CreateTranslation(position_);
    return scale * rot * trans;
}
