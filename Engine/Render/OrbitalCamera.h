#pragma once

#include "Camera.h"
#include "../Input/Device.h"
#include "../Input/Mouse.h"
#include "../Input/Keyboard.h"

namespace Engine::Render
{
    class OrbitalCamera final : public Camera
    {
    public:
        void Construct(Engine::Input::Device* pDevice, float fovDegrees, float aspect,
                       float nearPlane = 0.1f, float farPlane = 1000.f);

        float4x4 GetView() const override;
        float4x4 GetProjection() const override;
        void FixedUpdate() override;

        void SetTarget(float3 target) { target_ = target; }

    private:
        void OnMouse(const Engine::Input::Mouse::Event& e);
        void OnKeyboard(const Engine::Input::Keyboard::Event& e);

        Engine::Input::Device* pDevice_ = nullptr;

        float3 target_   = float3(0.f, 0.f, 0.f);
        float  distance_ = 40.f;
        float  yaw_      = 0.3f;
        float  pitch_    = 0.4f;
        float  orbitSpeed_ = 0.005f;
        float  zoomSpeed_  = 2.f;

        bool   rightMouseDown_ = false;

        float4x4 projection_{};
    };
}
