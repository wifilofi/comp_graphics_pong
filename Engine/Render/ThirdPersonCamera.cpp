#include "ThirdPersonCamera.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

using namespace Engine::Render;

void ThirdPersonCamera::Construct(Engine::Input::Device* pDevice, float fovDegrees,
                                  float aspect, float nearPlane, float farPlane)
{
    pDevice_ = pDevice;
    projection_ = float4x4::CreatePerspectiveFieldOfView(
        fovDegrees * static_cast<float>(M_PI) / 180.f, aspect, nearPlane, farPlane);

    pDevice_->MouseEvent.AddRaw(this, &ThirdPersonCamera::OnMouse);
}

float3 ThirdPersonCamera::GetEyePos() const
{
    const float x = distance_ * cosf(pitch_) * sinf(yaw_);
    const float y = distance_ * sinf(pitch_);
    const float z = distance_ * cosf(pitch_) * cosf(yaw_);
    return target_ + float3(x, y, z);
}

float4x4 ThirdPersonCamera::GetView() const
{
    return float4x4::CreateLookAt(GetEyePos(), target_, float3(0, 1, 0));
}

float4x4 ThirdPersonCamera::GetProjection() const
{
    return projection_;
}

void ThirdPersonCamera::OnMouse(const Engine::Input::Mouse::Event& e)
{
    yaw_   -= static_cast<float>(e.Translation.x) * lookSpeed_;
    pitch_ += static_cast<float>(e.Translation.y) * lookSpeed_;
    pitch_  = std::clamp(pitch_, 0.1f, 1.4f);
}

float3 ThirdPersonCamera::GetForwardDir() const
{
    return float3(-sinf(yaw_), 0.f, -cosf(yaw_));
}

float3 ThirdPersonCamera::GetRightDir() const
{
    return float3(cosf(yaw_), 0.f, -sinf(yaw_));
}
