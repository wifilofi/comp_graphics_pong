#include "SolarSystem.h"

#include "../../Engine/Render/Pipeline.h"
#include "../../Engine/Basic/Shapes/Box.h"
#include "../../Engine/Basic/Shapes/LowPolySphere.h"
#include <imgui.h>

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

    using ST = Basic::Components::Rendering3D::ShaderType;
    sphereRenderer_.Construct(pPipeline_, Basic::Shapes::LowPolySphere::Vertices(),
                                          Basic::Shapes::LowPolySphere::Indices(), ST::PerlinNoise);
    boxRenderer_   .Construct(pPipeline_, Basic::Shapes::Box::Vertices(),
                                          Basic::Shapes::Box::Indices(),            ST::PerlinNoise);

    BuildBodies();
}

void SolarSystem::BuildBodies()
{
    using P = SolarBody::Params;
    sun_.Construct(P{ShapeType::Sphere, {1.0f,0.9f,0.1f,1}, {1.0f,0.5f,0.0f,1},
                     SolarBody::ShaderType::PerlinNoise, 3.f, 0, 0, 0, 0.003f});
    SpawnPlanets(planetInput_);
}

void SolarSystem::SpawnPlanets(int n)
{
    using P = SolarBody::Params;

    planets_.clear();

    static constexpr float4 kColors[] = {
        {0.6f, 0.6f, 0.6f, 1},
        {0.9f, 0.8f, 0.5f, 1},
        {0.2f, 0.5f, 1.0f, 1},
        {0.8f, 0.3f, 0.1f, 1},
        {0.9f, 0.7f, 0.4f, 1},
        {0.8f, 0.6f, 0.3f, 1},
        {0.6f, 0.7f, 0.9f, 1},
        {0.5f, 0.8f, 0.7f, 1},
    };
    static constexpr float4 kColors2[] = {
        {0.15f, 0.10f, 0.10f, 1},
        {0.30f, 0.55f, 0.20f, 1},
        {0.05f, 0.15f, 0.40f, 1},
        {0.25f, 0.10f, 0.05f, 1},
        {0.40f, 0.25f, 0.05f, 1},
        {0.20f, 0.15f, 0.08f, 1},
        {0.90f, 0.95f, 1.00f, 1},
        {0.10f, 0.30f, 0.25f, 1},
    };

    const float raw  = n > 1 ? 1900.f / static_cast<float>(n) : 1900.f;
    const float step = raw < 2.f ? raw : 2.f;

    for (int i = 0; i < n; ++i)
    {
        const float fi    = static_cast<float>(i);
        const float orbit = 3.f + fi * step;
        const float scale = 0.8f + 0.25f * static_cast<float>(i % 6);
        const float speed = 0.025f / (1.f + fi * 0.28f);
        const float4& color  = kColors[i % 8];
        const float4& color2 = kColors2[i % 8];
        const ShapeType shape = (i % 4 == 1) ? ShapeType::Box : ShapeType::Sphere;

        float incl        = 0.f;
        float angleOffset = 0.f;

        if (formula_ == Formula::Spiral3D)
            incl = inclStep_ * fi;
        else
            angleOffset = angleStep_ * i;

        auto body = std::make_unique<SolarBody>();
        body->Construct(P{shape, color, color2,
                          SolarBody::ShaderType::PerlinNoise,
                          scale, orbit, speed, incl, 0.007f, angleOffset}, &sun_);
        planets_.push_back(std::move(body));
    }
}

void SolarSystem::FixedUpdate()
{
    sun_.FixedUpdate();
    for (auto& p : planets_) p->FixedUpdate();

    if (useFps_) fpsCamera_.FixedUpdate();
    else         orbCamera_.FixedUpdate();
}

void SolarSystem::Render(float /*delta*/)
{
    using OD = Basic::Components::Rendering3D::ObjectData;

    std::vector<OD> sphereInstances;
    std::vector<OD> boxInstances;

    sphereInstances.push_back(sun_.GetInstanceData());

    for (auto& p : planets_)
    {
        if (p->GetShape() == ShapeType::Sphere)
            sphereInstances.push_back(p->GetInstanceData());
        else
            boxInstances.push_back(p->GetInstanceData());
    }

    sphereRenderer_.DrawInstanced(sphereInstances);
    if (!boxInstances.empty())
        boxRenderer_.DrawInstanced(boxInstances);
}

void SolarSystem::RenderUI()
{
    ImGui::Begin("Solar System");

    ImGui::InputInt("Planet Count", &planetInput_);
    if (planetInput_ < 1)    planetInput_ = 1;
    if (planetInput_ > 1000) planetInput_ = 1000;

    int formulaIdx = (formula_ == Formula::Spiral2D) ? 1 : 0;
    if (ImGui::Combo("Formula", &formulaIdx, "3D Spiral\0""2D Spiral\0"))
        formula_ = (formulaIdx == 1) ? Formula::Spiral2D : Formula::Spiral3D;

    if (formula_ == Formula::Spiral3D)
        ImGui::SliderFloat("Tilt per planet (rad)", &inclStep_, 0.0f, 1.0f);
    else
        ImGui::SliderFloat("Angle per planet (rad)", &angleStep_, 0.1f, 6.28f);

    if (ImGui::Button("Spawn"))
        SpawnPlanets(planetInput_);

    ImGui::Text("%d planets active", static_cast<int>(planets_.size()));
    ImGui::End();
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
