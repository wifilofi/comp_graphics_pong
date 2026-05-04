#pragma once

#include "Camera.h"
#include "../Input/Device.h"
#include "../Input/Mouse.h"

namespace Engine::Render
{
    class ThirdPersonCamera final : public Camera
    {
    public:
        void Construct(Engine::Input::Device* pDevice, float fovDegrees, float aspect,
                       float nearPlane = 0.1f, float farPlane = 1000.f);

        float4x4 GetView()       const override;
        float4x4 GetProjection() const override;
        float3   GetEyePos()     const override;

        void SetTarget(float3 target) { target_ = target; }

        float3 GetForwardDir() const;
        float3 GetRightDir()   const;

    private:
        void OnMouse(const Engine::Input::Mouse::Event& e);

        Engine::Input::Device* pDevice_ = nullptr;

        float3   target_     = {};
        float    yaw_        = 0.f;
        float    pitch_      = 0.4f;
        float    distance_   = 40.f;
        float    lookSpeed_  = 0.005f;
        float4x4 projection_{};
    };
}
