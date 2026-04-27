#pragma once

#include <vector>
#include <memory>
#include <string>

#include "../../Engine/Render/Renderer.h"
#include "../../Engine/Update/FixedAble.h"
#include "../../Engine/Input/Device.h"
#include "../../Engine/Basic/Components/Rendering3D.h"
#include "../../Engine/Render/OrbitalCamera.h"
#include "../../Engine/Lib/Types.h"

namespace Engine::Render { class Pipeline; }

namespace Katamari
{
    class KatamariWorld : public Engine::Render::Renderer,
                          public Engine::Update::FixedAble
    {
    public:
        void Construct(Engine::Render::Pipeline* pPipeline) override;
        void FixedUpdate() override;
        void Render(float delta) override;
        void RenderUI() override;

        void SetInputDevice(Engine::Input::Device* pDevice) { pDevice_ = pDevice; }

    private:
        struct Pickup {
            float3 pos;
            float  radius;
            float4 color, color2;
            bool   isSphere;
            bool   absorbed   = false;
            float3 localOffset;  // ball-local space (set on absorb)
        };

        struct FbxPickup {
            float3 pos;
            float  radius = 1.2f;
            bool   absorbed = false;
            float3 localOffset;
        };

        void SpawnPickups();
        void UpdateBall();
        void CheckCollisions();
        void AbsorbPickup(Pickup& p);
        void AbsorbFbxPickup(FbxPickup& p);
        void LoadMesh(const std::string& path);
        void OpenFileBrowser();

        Engine::Render::Pipeline*         pPipeline_ = nullptr;
        Engine::Input::Device*            pDevice_   = nullptr;
        Engine::Render::OrbitalCamera     camera_;

        float3   ballPos_    = {0.f, 0.f, 0.f};
        float    ballRadius_ = 1.5f;
        float3   ballVel_    = {0.f, 0.f, 0.f};
        float4x4 rollMatrix_;
        int      absorbedCount_ = 0;

        Basic::Components::Rendering3D ballRenderer_;
        Basic::Components::Rendering3D planeRenderer_;
        Basic::Components::Rendering3D spherePickupRenderer_;
        Basic::Components::Rendering3D boxPickupRenderer_;

        std::unique_ptr<Basic::Components::Rendering3D> fbxRenderer_;

        std::vector<Pickup>    pickups_;
        std::vector<FbxPickup> fbxPickups_;

        char fbxPathBuf_[512] = {};
    };
}
