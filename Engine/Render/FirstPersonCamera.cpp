#include "FirstPersonCamera.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

using namespace Engine::Render;
using Keys = Engine::Input::Keyboard::Keys;

void FirstPersonCamera::Construct(Engine::Input::Device* pDevice, float fovDegrees,
                                  float aspect, float nearPlane, float farPlane)
{
    pDevice_ = pDevice;
    aspect_ = aspect;
    nearPlane_ = nearPlane;
    farPlane_ = farPlane;

    const float fovRadians = fovDegrees * static_cast<float>(M_PI) / 180.f;
    perspectiveMatrix_ = float4x4::CreatePerspectiveFieldOfView(
        fovRadians, aspect_, nearPlane_, farPlane_);

    const float width = orthoSize_ * aspect_;
    const float height = orthoSize_;
    orthographicMatrix_ = float4x4::CreateOrthographic(width, height, nearPlane_, farPlane_);

    pDevice_->MouseEvent.AddRaw(this, &FirstPersonCamera::OnMouse);
}

float4x4 FirstPersonCamera::GetView() const
{
    const float4x4 rotation = float4x4::CreateFromYawPitchRoll(yaw_, pitch_, 0.f);
    const float3 forward = float3::TransformNormal(float3(0, 0, 1), rotation);
    const float3 up      = float3::TransformNormal(float3(0, 1, 0), rotation);
    return float4x4::CreateLookAt(position_, position_ + forward, up);
}

float4x4 FirstPersonCamera::GetProjection() const
{
    return orthoActive_ ? orthographicMatrix_ : perspectiveMatrix_;
}

void FirstPersonCamera::FixedUpdate()
{
    orthoActive_ = pDevice_->IsKeyDown(Keys::LeftShift);

    const float4x4 rotation = float4x4::CreateFromYawPitchRoll(yaw_*-1, pitch_, 0.f);
    const float3 forward = float3::TransformNormal(float3(0, 0, 1), rotation);
    const float3 right   = float3::TransformNormal(float3(1, 0, 0), rotation);

    if (pDevice_->IsKeyDown(Keys::W)) position_ += forward * moveSpeed_;
    if (pDevice_->IsKeyDown(Keys::S)) position_ -= forward * moveSpeed_;
    if (pDevice_->IsKeyDown(Keys::D)) position_ -= right   * moveSpeed_;
    if (pDevice_->IsKeyDown(Keys::A)) position_ += right   * moveSpeed_;
    if (pDevice_->IsKeyDown(Keys::E)) position_.y += moveSpeed_;
    if (pDevice_->IsKeyDown(Keys::Q)) position_.y -= moveSpeed_;
}

void FirstPersonCamera::OnMouse(const Engine::Input::Mouse::Event& e)
{
    yaw_   += static_cast<float>(e.Translation.x) * lookSpeed_;
    pitch_ += static_cast<float>(e.Translation.y) * lookSpeed_;
    pitch_  = std::clamp(pitch_, -1.4f, 1.4f);
}