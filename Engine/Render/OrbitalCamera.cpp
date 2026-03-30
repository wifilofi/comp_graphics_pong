#include "OrbitalCamera.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

using namespace Engine::Render;
using Keys    = Engine::Input::Keyboard::Keys;
using Buttons = Engine::Input::Keyboard::Buttons;

void OrbitalCamera::Construct(Engine::Input::Device* pDevice, float fovDegrees,
                              float aspect, float nearPlane, float farPlane)
{
    pDevice_ = pDevice;
    projection_ = float4x4::CreatePerspectiveFieldOfView(
        fovDegrees * static_cast<float>(M_PI) / 180.f, aspect, nearPlane, farPlane);

    pDevice_->MouseEvent.AddRaw(this, &OrbitalCamera::OnMouse);
    pDevice_->KeyboardEvent.AddRaw(this, &OrbitalCamera::OnKeyboard);
}

float4x4 OrbitalCamera::GetView() const
{
    const float x = distance_ * cosf(pitch_) * sinf(yaw_);
    const float y = distance_ * sinf(pitch_);
    const float z = distance_ * cosf(pitch_) * cosf(yaw_);
    const float3 eye = target_ + float3(x, y, z);
    return float4x4::CreateLookAt(eye, target_, float3(0, 1, 0));
}

float4x4 OrbitalCamera::GetProjection() const
{
    return projection_;
}

void OrbitalCamera::FixedUpdate()
{
    // Keyboard zoom
    if (pDevice_->IsKeyDown(Keys::E)) distance_ = max(2.f, distance_ - zoomSpeed_);
    if (pDevice_->IsKeyDown(Keys::Q)) distance_ += zoomSpeed_;
}

void OrbitalCamera::OnMouse(const Engine::Input::Mouse::Event& e)
{
    if (rightMouseDown_)
    {
        yaw_   -= static_cast<float>(e.Translation.x) * orbitSpeed_;
        pitch_ += static_cast<float>(e.Translation.y) * orbitSpeed_;
        pitch_  = std::clamp(pitch_, -1.4f, 1.4f);
    }

    // Scroll wheel zoom
    if (e.WheelDelta != 0)
    {
        const float dir = e.WheelDelta > 0 ? -1.f : 1.f;
        distance_ = max(2.f, distance_ + dir * zoomSpeed_);
    }
}

void OrbitalCamera::OnKeyboard(const Engine::Input::Keyboard::Event& e)
{
    if (e.Code == Keys::MouseRightButton)
        rightMouseDown_ = (e.Button == Buttons::Down);
}
