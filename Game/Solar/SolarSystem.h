#pragma once

#include <memory>
#include <vector>
#include "SolarBody.h"
#include "../../Engine/Render/Renderer.h"
#include "../../Engine/Update/FixedAble.h"
#include "../../Engine/Input/Device.h"
#include "../../Engine/Input/Keyboard.h"
#include "../../Engine/Render/FirstPersonCamera.h"
#include "../../Engine/Render/OrbitalCamera.h"

namespace Engine::Render { class Pipeline; }

namespace Solar
{
    class SolarSystem : public Engine::Render::Renderer,
                        public Engine::Update::FixedAble
    {
    public:
        void Construct(Engine::Render::Pipeline* pPipeline) override;
        void FixedUpdate() override;
        void Render(float delta) override;
        void RenderUI() override;

        void SetInputDevice(Engine::Input::Device* pDevice) { pDevice_ = pDevice; }

    private:
        void OnKeyboard(const Engine::Input::Keyboard::Event& e);
        void SwitchCamera();
        void BuildBodies();
        void SpawnPlanets(int n);

        Engine::Render::Pipeline*          pPipeline_ = nullptr;
        Engine::Input::Device*             pDevice_   = nullptr;

        Engine::Render::FirstPersonCamera  fpsCamera_;
        Engine::Render::OrbitalCamera      orbCamera_;
        bool                               useFps_ = false;

        SolarBody                                   sun_;
        std::vector<std::unique_ptr<SolarBody>>     planets_;
        int                                         planetInput_ = 8;
    };
}
