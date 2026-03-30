#include "SolarSystem.h"

#include "../../Engine/Render/Pipeline.h"

using namespace Solar;
using Keys    = Engine::Input::Keyboard::Keys;
using Buttons = Engine::Input::Keyboard::Buttons;

static constexpr float kFov    = 60.f;
static constexpr float kAspect = 750.f / 500.f;
static constexpr float kNear   = 0.1f;
static constexpr float kFar    = 2000.f;

void SolarSystem::Construct(Engine::Render::Pipeline* pPipeline)
{
    pPipeline_ = pPipeline;

    fpsCamera_.Construct(pDevice_, kFov, kAspect, kNear, kFar);
    orbCamera_.Construct(pDevice_, kFov, kAspect, kNear, kFar);
    pPipeline_->SetCamera(&orbCamera_);

    pDevice_->KeyboardEvent.AddRaw(this, &SolarSystem::OnKeyboard);

    BuildBodies();
}

void SolarSystem::BuildBodies()
{
    using P = SolarBody::Params;

    // Sun
    sun_.Construct(pPipeline_, P{ShapeType::Sphere, {1.0f,0.9f,0.1f,1}, 3.f, 0, 0, 0, 0.003f});

    // Inner planets
    mercury_.Construct(pPipeline_, P{ShapeType::Sphere,{0.6f,0.6f,0.6f,1}, 0.4f, 5.f,  0.020f, 0.10f, 0.008f}, &sun_);
    venus_  .Construct(pPipeline_, P{ShapeType::Box,   {0.9f,0.8f,0.5f,1}, 0.9f, 8.f,  0.013f, 0.05f, 0.004f}, &sun_);

    // Earth + Moon
    earth_  .Construct(pPipeline_, P{ShapeType::Sphere,{0.2f,0.5f,1.0f,1}, 1.0f, 12.f, 0.010f, 0.03f, 0.007f}, &sun_);
    moon_   .Construct(pPipeline_, P{ShapeType::Sphere,{0.7f,0.7f,0.7f,1}, 0.3f, 2.2f, 0.050f, 0.08f, 0.005f}, &earth_);

    // Mars + moons
    mars_   .Construct(pPipeline_, P{ShapeType::Sphere,{0.8f,0.3f,0.1f,1}, 0.7f, 17.f, 0.008f, 0.02f, 0.009f}, &sun_);
    phobos_ .Construct(pPipeline_, P{ShapeType::Box,   {0.5f,0.4f,0.3f,1}, 0.2f, 1.5f, 0.080f, 0.00f, 0.012f}, &mars_);
    deimos_ .Construct(pPipeline_, P{ShapeType::Sphere,{0.6f,0.5f,0.4f,1}, 0.15f,2.5f, 0.040f, 0.04f, 0.010f}, &mars_);

    // Jupiter + moons
    jupiter_.Construct(pPipeline_, P{ShapeType::Sphere,{0.9f,0.7f,0.4f,1}, 2.0f, 25.f, 0.005f, 0.01f, 0.006f}, &sun_);
    io_     .Construct(pPipeline_, P{ShapeType::Box,   {0.9f,0.8f,0.2f,1}, 0.4f, 3.5f, 0.030f, 0.06f, 0.011f}, &jupiter_);
    europa_ .Construct(pPipeline_, P{ShapeType::Sphere,{0.7f,0.8f,0.9f,1}, 0.35f,5.0f, 0.022f, 0.03f, 0.008f}, &jupiter_);
}

void SolarSystem::FixedUpdate()
{
    sun_    .FixedUpdate();
    mercury_.FixedUpdate();
    venus_  .FixedUpdate();
    earth_  .FixedUpdate();
    moon_   .FixedUpdate();
    mars_   .FixedUpdate();
    phobos_ .FixedUpdate();
    deimos_ .FixedUpdate();
    jupiter_.FixedUpdate();
    io_     .FixedUpdate();
    europa_ .FixedUpdate();

    if (useFps_) fpsCamera_.FixedUpdate();
    else         orbCamera_.FixedUpdate();
}

void SolarSystem::Render(float delta)
{
    sun_    .Render(delta);
    mercury_.Render(delta);
    venus_  .Render(delta);
    earth_  .Render(delta);
    moon_   .Render(delta);
    mars_   .Render(delta);
    phobos_ .Render(delta);
    deimos_ .Render(delta);
    jupiter_.Render(delta);
    io_     .Render(delta);
    europa_ .Render(delta);
}

void SolarSystem::OnKeyboard(const Engine::Input::Keyboard::Event& e)
{
    if (e.Code == Keys::Tab && e.Button == Buttons::Down)
        SwitchCamera();
}

void SolarSystem::SwitchCamera()
{
    useFps_ = !useFps_;
    pPipeline_->SetCamera(useFps_ ? static_cast<Engine::Render::Camera*>(&fpsCamera_)
                                  : static_cast<Engine::Render::Camera*>(&orbCamera_));
}
