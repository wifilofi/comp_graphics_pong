#pragma once

#include <vector>
#include "../../Engine/Lib/Types.h"
#include "../../Engine/Basic/Components/Rendering3D.h"

namespace Engine::Render { class Pipeline; }

namespace Solar
{
    enum class ShapeType { Sphere, Box };

    class SolarBody
    {
    public:
        struct Params
        {
            ShapeType shape       = ShapeType::Sphere;
            float4    color       = float4(1, 1, 1, 1);
            float     scale       = 1.f;
            float     orbitRadius = 0.f;
            float     orbitSpeed  = 0.f;
            float     orbitInclination = 0.f;
            float     selfRotSpeed = 0.01f;
        };

        void Construct(Engine::Render::Pipeline* pPipeline, const Params& params,
                       const SolarBody* parent = nullptr);

        void FixedUpdate();
        void Render(float delta);

        float3 GetPosition() const { return position_; }

    private:
        float4x4 ComputeModelMatrix() const;

        Basic::Components::Rendering3D rendering_;

        const SolarBody* parent_    = nullptr;
        float3           position_  = float3(0, 0, 0);
        float            scale_     = 1.f;
        float4           color_     = float4(1, 1, 1, 1);
        float            orbitRadius_     = 0.f;
        float            orbitSpeed_      = 0.f;
        float            orbitInclination_= 0.f;
        float            orbitAngle_      = 0.f;
        float            selfRotSpeed_    = 0.01f;
        float            selfRotAngle_    = 0.f;
    };
}
