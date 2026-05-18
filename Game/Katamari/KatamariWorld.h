#pragma once

#include <vector>
#include <memory>
#include <string>

#include "../../Engine/Render/Renderer.h"
#include "../../Engine/Update/FixedAble.h"
#include "../../Engine/Input/Device.h"
#include "../../Engine/Basic/Components/Rendering3D.h"
#include "../../Engine/Render/ThirdPersonCamera.h"
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
            std::vector<float3>  meshVerts;
            std::vector<int32>   meshIndices;
        };

        struct ShotLight
        {
            float3 pos;
            float3 vel;
            float3 color;
            float  life = 8.f;  // seconds at 60fps ticks
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
        Engine::Render::ThirdPersonCamera  camera_;

        float3   ballPos_    = {0.f, 0.f, 0.f};
        float    ballRadius_ = 3.f;
        float3   ballVel_    = {0.f, 0.f, 0.f};
        float    ballVelY_   = 0.f;
        float    ballY_      = 0.f;  // height above ground (0 = on ground)
        int      jumpsLeft_  = 2;
        bool     spaceWasDown_ = false;
        bool     lmbWasDown_   = false;

        std::vector<ShotLight> shotLights_;
        float4x4 rollMatrix_;
        int      absorbedCount_ = 0;

        Basic::Components::Rendering3D ballRenderer_;
        Basic::Components::Rendering3D planeRenderer_;
        Basic::Components::Rendering3D spherePickupRenderer_;
        Basic::Components::Rendering3D boxPickupRenderer_;
        Basic::Components::Rendering3D shotLightRenderer_;

        std::unique_ptr<Basic::Components::Rendering3D> fbxMeshRenderer_;
        ID3D11ShaderResourceView*                        fbxTexSRV_ = nullptr;

        std::vector<Pickup>    pickups_;
        std::vector<FbxPickup> fbxPickups_;

        ID3D11ShaderResourceView*                        ballTexSRV_   = nullptr;
        ID3D11ShaderResourceView*                        cloudsTexSRV_ = nullptr;
        ID3D11ShaderResourceView*                        grassTexSRV_  = nullptr;

        ID3D11Texture2D*          pShadowTex_     = nullptr;
        ID3D11DepthStencilView*   pShadowDSV_     = nullptr;
        ID3D11ShaderResourceView* pShadowSRV_     = nullptr;
        ID3D11SamplerState*       pShadowSampler_ = nullptr;

        char fbxPathBuf_[512] = {};
    };
}
