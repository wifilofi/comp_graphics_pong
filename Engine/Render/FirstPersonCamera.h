#pragma once

#include "Camera.h"
#include "../Input/Device.h"
#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"

namespace Engine::Render
{
    class FirstPersonCamera final : public Camera
    {
    public:
        void Construct(Engine::Input::Device* pDevice, float fovDegrees, float aspect,
                       float nearPlane = 0.1f, float farPlane = 1000.f);

        float4x4 GetView() const override;
        float4x4 GetProjection() const override;
        void FixedUpdate() override;

    private:
        void OnMouse(const Engine::Input::Mouse::Event& e);

        Engine::Input::Device* pDevice_ = nullptr;

        float3 position_   = float3(0.f, 5.f, -20.f);
        float  yaw_        = 0.f;
        float  pitch_      = 0.f;
        float  moveSpeed_  = 0.2f;
        float  lookSpeed_  = 0.003f;

        float4x4 projection_{};
    };
}
