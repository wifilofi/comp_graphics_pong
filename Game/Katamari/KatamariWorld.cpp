#include "KatamariWorld.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <cstdlib>
#include <cstring>

#include <imgui.h>
#include <commdlg.h>
#include <WICTextureLoader.h>
#include <filesystem>

#include "../../Engine/Render/Pipeline.h"
#include "../../Engine/Basic/Shapes/LowPolySphere.h"
#include "../../Engine/Basic/Shapes/Box.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace Katamari;
using Keys    = Engine::Input::Keyboard::Keys;

static constexpr float kFov       = 60.f;
static constexpr float kAspect    = 750.f / 500.f;
static constexpr float kNear      = 0.1f;
static constexpr float kFar       = 500.f;
static constexpr float kAccel     = 0.015f;
static constexpr float kFriction  = 0.92f;
static constexpr float kMaxSpeed  = 0.35f;
static constexpr float kPlaneHalf = 400.f;
static constexpr float kJumpForce    = 0.5f;
static constexpr float kRecoilForce  = 0.3f;
static constexpr float kGravityRise   = 0.012f;
static constexpr float kGravityFall   = 0.030f;
static constexpr int   kShadowMapSize = 2048;


void KatamariWorld::Construct(Engine::Render::Pipeline* pPipeline)
{
    pPipeline_  = pPipeline;
    rollMatrix_ = float4x4::Identity;

    camera_.Construct(pDevice_, kFov, kAspect, kNear, kFar);
    camera_.SetTarget(float3(0.f, 0.f, 0.f));
    pPipeline_->SetCamera(&camera_);

    using ST = Basic::Components::Rendering3D::ShaderType;
    auto sphereV = Basic::Shapes::LowPolySphere::Vertices();
    auto sphereI = Basic::Shapes::LowPolySphere::Indices();
    auto boxV    = Basic::Shapes::Box::Vertices();
    auto boxI    = Basic::Shapes::Box::Indices();

    {
        namespace fs = std::filesystem;
        const std::wstring ballTex   = (fs::path(__FILE__).parent_path() / "Textures" / "T_player_eshka.png").wstring();
        const std::wstring cloudsTex = (fs::path(__FILE__).parent_path() / "Textures" / "T_clouds.png").wstring();
        const std::wstring grassTex  = (fs::path(__FILE__).parent_path() / "Textures" / "T_grass.png").wstring();
        DirectX::CreateWICTextureFromFile(pPipeline_->GetDevice(), nullptr, ballTex.c_str(),   nullptr, &ballTexSRV_);
        DirectX::CreateWICTextureFromFile(pPipeline_->GetDevice(), nullptr, cloudsTex.c_str(), nullptr, &cloudsTexSRV_);
        DirectX::CreateWICTextureFromFile(pPipeline_->GetDevice(), nullptr, grassTex.c_str(),  nullptr, &grassTexSRV_);
    }
    ballRenderer_        .Construct(pPipeline_, sphereV, sphereI, ST::PhongTex, ballTexSRV_);
    shotLightRenderer_   .Construct(pPipeline_, sphereV, sphereI, ST::Glow);
    spherePickupRenderer_.Construct(pPipeline_, sphereV, sphereI, ST::PhongTex, cloudsTexSRV_);
    planeRenderer_       .Construct(pPipeline_, boxV,    boxI,    ST::PhongTex, grassTexSRV_);
    boxPickupRenderer_   .Construct(pPipeline_, boxV,    boxI,    ST::Phong);

    SpawnPickups();

    {
        auto* device = pPipeline_->GetDevice();

        D3D11_TEXTURE2D_DESC texDesc = {};
        texDesc.Width              = kShadowMapSize;
        texDesc.Height             = kShadowMapSize;
        texDesc.MipLevels          = 1;
        texDesc.ArraySize          = 1;
        texDesc.Format             = DXGI_FORMAT_R32_TYPELESS;
        texDesc.SampleDesc.Count   = 1;
        texDesc.Usage              = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        device->CreateTexture2D(&texDesc, nullptr, &pShadowTex_);

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format             = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
        device->CreateDepthStencilView(pShadowTex_, &dsvDesc, &pShadowDSV_);

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format                    = DXGI_FORMAT_R32_FLOAT;
        srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels       = 1;
        device->CreateShaderResourceView(pShadowTex_, &srvDesc, &pShadowSRV_);

        D3D11_SAMPLER_DESC sd = {};
        sd.Filter         = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
        sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
        sd.BorderColor[0] = sd.BorderColor[1] = sd.BorderColor[2] = sd.BorderColor[3] = 1.f;
        sd.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
        sd.MaxLOD         = D3D11_FLOAT32_MAX;
        device->CreateSamplerState(&sd, &pShadowSampler_);
    }
}



void KatamariWorld::SpawnPickups()
{
    pickups_.clear();
    absorbedCount_ = 0;

    static const float4 kColors[]  = {
        {0.9f, 0.3f, 0.2f, 1}, {0.2f, 0.8f, 0.4f, 1},
        {0.3f, 0.4f, 0.9f, 1}, {0.9f, 0.8f, 0.2f, 1},
        {0.8f, 0.3f, 0.8f, 1}, {0.2f, 0.8f, 0.8f, 1},
    };
    static const float4 kColors2[] = {
        {0.4f, 0.1f, 0.1f, 1}, {0.1f, 0.3f, 0.1f, 1},
        {0.1f, 0.1f, 0.4f, 1}, {0.4f, 0.3f, 0.1f, 1},
        {0.3f, 0.1f, 0.3f, 1}, {0.1f, 0.3f, 0.3f, 1},
    };

    srand(42);
    for (int i = 0; i < 40; ++i)
    {
        const float rx = (rand() % 7200 - 3600) / 100.f;
        const float rz = (rand() % 7200 - 3600) / 100.f;
        if (fabsf(rx) < 5.f && fabsf(rz) < 5.f) continue;

        Pickup p;
        p.radius   = 0.25f + (rand() % 180) / 100.f;
        p.pos      = float3(rx, p.radius, rz);
        p.color    = kColors[i % 6];
        p.color2   = kColors2[i % 6];
        p.isSphere = (rand() % 2 == 0);
        pickups_.push_back(p);
    }
}


void KatamariWorld::FixedUpdate()
{
    UpdateBall();
    CheckCollisions();
    camera_.SetTarget(float3(ballPos_.x, ballRadius_ + ballY_, ballPos_.z));

    // shoot light on LMB press
    const bool lmbDown = pDevice_->IsKeyDown(Keys::MouseLeftButton);
    if (lmbDown && !lmbWasDown_)
    {
        static const float3 kLightColors[] = {
            {1.f, 0.4f, 0.1f}, {0.2f, 0.6f, 1.f}, {0.4f, 1.f, 0.3f},
            {1.f, 0.2f, 0.8f}, {1.f, 0.9f, 0.2f}, {0.2f, 1.f, 0.9f},
        };
        static int colorIdx = 0;

        const float3 ballCenter(ballPos_.x, ballRadius_ + ballY_, ballPos_.z);
        const float3 fwd = camera_.GetForwardDir();

        ShotLight sl;
        sl.pos   = ballCenter + float3(0.f, ballRadius_, 0.f);
        sl.vel   = fwd * 3.f;
        sl.color = kLightColors[colorIdx++ % 6];
        sl.life  = 300.f;

        if (static_cast<int>(shotLights_.size()) < Basic::Components::Rendering3D::kMaxLights - 1)
            shotLights_.push_back(sl);

        ballVel_.x = std::clamp(ballVel_.x - fwd.x * kRecoilForce, -kMaxSpeed, kMaxSpeed);
        ballVel_.z = std::clamp(ballVel_.z - fwd.z * kRecoilForce, -kMaxSpeed, kMaxSpeed);
    }
    lmbWasDown_ = lmbDown;

    // move and age shot lights
    for (auto& sl : shotLights_)
    {
        sl.pos  += sl.vel;
        sl.life -= 1.f;
    }
    shotLights_.erase(
        std::remove_if(shotLights_.begin(), shotLights_.end(),
                       [](const ShotLight& s){ return s.life <= 0.f; }),
        shotLights_.end());
}

void KatamariWorld::UpdateBall()
{
    const float3 fwd   = camera_.GetForwardDir();
    const float3 right = camera_.GetRightDir();

    float3 accel(0.f, 0.f, 0.f);
    if (pDevice_->IsKeyDown(Keys::W)) accel += fwd   * kAccel;
    if (pDevice_->IsKeyDown(Keys::S)) accel -= fwd   * kAccel;
    if (pDevice_->IsKeyDown(Keys::A)) accel -= right * kAccel;
    if (pDevice_->IsKeyDown(Keys::D)) accel += right * kAccel;

    const float ax = accel.x, az = accel.z;

    ballVel_.x = std::clamp(ballVel_.x + ax, -kMaxSpeed, kMaxSpeed) * kFriction;
    ballVel_.z = std::clamp(ballVel_.z + az, -kMaxSpeed, kMaxSpeed) * kFriction;

    ballPos_.x = std::clamp(ballPos_.x + ballVel_.x, -kPlaneHalf + ballRadius_, kPlaneHalf - ballRadius_);
    ballPos_.z = std::clamp(ballPos_.z + ballVel_.z, -kPlaneHalf + ballRadius_, kPlaneHalf - ballRadius_);

    // jump
    const bool spaceDown = pDevice_->IsKeyDown(Keys::Space);
    if (spaceDown && !spaceWasDown_ && jumpsLeft_ > 0)
    {
        ballVelY_   = kJumpForce;
        --jumpsLeft_;
    }
    spaceWasDown_ = spaceDown;

    // gravity + vertical integration
    ballVelY_ -= (ballVelY_ > 0.f ? kGravityRise : kGravityFall);
    ballY_    += ballVelY_;
    if (ballY_ <= 0.f)
    {
        ballY_     = 0.f;
        ballVelY_  = 0.f;
        jumpsLeft_ = 2;
    }

    const float speed = sqrtf(ballVel_.x * ballVel_.x + ballVel_.z * ballVel_.z);
    if (speed > 0.0005f)
    {
        float3 moveDir(ballVel_.x, 0.f, ballVel_.z);
        moveDir.Normalize();
        // roll axis = moveDir * up
        float3 rollAxis(moveDir.z, 0.f, -moveDir.x);
        const float angle = speed / ballRadius_;
        rollMatrix_ = rollMatrix_ * float4x4::CreateFromAxisAngle(rollAxis, angle);
    }
}

static float3 ClosestPointOnTriangle(float3 p, float3 a, float3 b, float3 c)
{
    const float3 ab = b - a, ac = c - a, ap = p - a;
    const float d1 = ab.Dot(ap), d2 = ac.Dot(ap);
    if (d1 <= 0.f && d2 <= 0.f) return a;

    const float3 bp = p - b;
    const float d3 = ab.Dot(bp), d4 = ac.Dot(bp);
    if (d3 >= 0.f && d4 <= d3) return b;

    const float3 cp = p - c;
    const float d5 = ab.Dot(cp), d6 = ac.Dot(cp);
    if (d6 >= 0.f && d5 <= d6) return c;

    const float vc = d1 * d4 - d3 * d2;
    if (vc <= 0.f && d1 >= 0.f && d3 <= 0.f)
        return a + ab * (d1 / (d1 - d3));

    const float vb = d5 * d2 - d1 * d6;
    if (vb <= 0.f && d2 >= 0.f && d6 <= 0.f)
        return a + ac * (d2 / (d2 - d6));

    const float va = d3 * d6 - d5 * d4;
    if (va <= 0.f && (d4 - d3) >= 0.f && (d5 - d6) >= 0.f)
        return b + (c - b) * ((d4 - d3) / ((d4 - d3) + (d5 - d6)));

    const float denom = 1.f / (va + vb + vc);
    return a + ab * (vb * denom) + ac * (vc * denom);
}

static bool SphereIntersectsMesh(float3 ballCenter, float ballRadius,
                                  const std::vector<float3>& meshVerts,
                                  const std::vector<int32>& meshIndices,
                                  float3 pickupPos, float pickupScale)
{
    // transform ball center into pickup local space (translate + uniform scale, no rotation)
    const float3 localCenter = (ballCenter - pickupPos) / pickupScale;
    const float  localRadius = ballRadius / pickupScale;
    const float  r2          = localRadius * localRadius;

    for (size_t i = 0; i + 2 < meshIndices.size(); i += 3)
    {
        const float3& a = meshVerts[meshIndices[i]];
        const float3& b = meshVerts[meshIndices[i + 1]];
        const float3& c = meshVerts[meshIndices[i + 2]];
        const float3  closest = ClosestPointOnTriangle(localCenter, a, b, c);
        if ((closest - localCenter).LengthSquared() <= r2)
            return true;
    }
    return false;
}

void KatamariWorld::CheckCollisions()
{
    const float3 ballCenter(ballPos_.x, ballRadius_ + ballY_, ballPos_.z);

    for (auto& p : pickups_)
    {
        if (p.absorbed) continue;
        const float3 pickupCenter(p.pos.x, p.radius, p.pos.z);
        const float  dist = (ballCenter - pickupCenter).Length();
        if (dist < ballRadius_ + p.radius && ballRadius_ >= p.radius)
            AbsorbPickup(p);
    }

    for (auto& p : fbxPickups_)
    {
        if (p.absorbed) continue;
        const float3 pickupCenter(p.pos.x, p.radius, p.pos.z);
        // broad phase
        const float dist = (ballCenter - pickupCenter).Length();
        if (dist > ballRadius_ + p.radius * 2.f) continue;
        // narrow phase: actual mesh triangles
        if (SphereIntersectsMesh(ballCenter, ballRadius_, p.meshVerts, p.meshIndices, pickupCenter, p.radius)
            && ballRadius_ >= p.radius)
            AbsorbFbxPickup(p);
    }
}

void KatamariWorld::AbsorbPickup(Pickup& p)
{
    const float3 ballCenter(ballPos_.x, ballRadius_ + ballY_, ballPos_.z);
    float3 dir = float3(p.pos.x, p.radius, p.pos.z) - ballCenter;
    dir.Normalize();
    const float3 worldOffset = dir * (ballRadius_ + p.radius);
    p.localOffset = float3::TransformNormal(worldOffset, rollMatrix_.Invert());
    p.absorbed    = true;
    ballRadius_  += p.radius * 0.04f;
    ++absorbedCount_;
}

void KatamariWorld::AbsorbFbxPickup(FbxPickup& p)
{
    const float3 ballCenter(ballPos_.x, ballRadius_ + ballY_, ballPos_.z);
    float3 dir = float3(p.pos.x, p.radius, p.pos.z) - ballCenter;
    dir.Normalize();
    const float3 worldOffset = dir * (ballRadius_ + p.radius);
    p.localOffset = float3::TransformNormal(worldOffset, rollMatrix_.Invert());
    p.absorbed    = true;
    ballRadius_  += p.radius * 0.04f;
    ++absorbedCount_;
}


void KatamariWorld::Render(float /*delta*/)
{
    using OD = Basic::Components::Rendering3D::ObjectData;
    using LD = Basic::Components::Rendering3D::LightData;

    auto* ctx = pPipeline_->GetDeviceContext();
    const float3 ballCenter(ballPos_.x, ballRadius_ + ballY_, ballPos_.z);

    // Build all instance data (shared by shadow pass and main pass)
    OD planeOD, ballOD;

    planeOD.model  = (float4x4::CreateScale(kPlaneHalf, 0.2f, kPlaneHalf) *
                      float4x4::CreateTranslation(0.f, -0.1f, 0.f)).Transpose();
    planeOD.color  = float4(1.f, 1.f, 1.f, 1.f);
    planeOD.color2 = float4(0.f, 0.f, 0.f, 80.f);

    ballOD.model  = (float4x4::CreateScale(ballRadius_) *
                     rollMatrix_ *
                     float4x4::CreateTranslation(ballCenter)).Transpose();
    ballOD.color  = float4(0.92f, 0.87f, 0.82f, 1.f);
    ballOD.color2 = float4(0.45f, 0.32f, 0.20f, 1.f);

    std::vector<OD> sphereODs, boxODs;
    for (const auto& p : pickups_)
    {
        float3   worldPos;
        float4x4 rot = float4x4::Identity;
        if (p.absorbed)
        {
            worldPos = ballCenter + float3::TransformNormal(p.localOffset, rollMatrix_);
            rot      = rollMatrix_;
        }
        else
        {
            worldPos = float3(p.pos.x, p.radius, p.pos.z);
        }
        OD od;
        od.model  = (float4x4::CreateScale(p.radius) * rot *
                     float4x4::CreateTranslation(worldPos)).Transpose();
        od.color  = p.color;
        od.color2 = p.color2;
        if (p.isSphere) sphereODs.push_back(od);
        else            boxODs.push_back(od);
    }

    std::vector<OD> fbxODs;
    if (fbxMeshRenderer_ && !fbxPickups_.empty())
    {
        for (const auto& p : fbxPickups_)
        {
            float3   worldPos;
            float4x4 rot = float4x4::Identity;
            if (p.absorbed)
            {
                worldPos = ballCenter + float3::TransformNormal(p.localOffset, rollMatrix_);
                rot      = rollMatrix_;
            }
            else
            {
                worldPos = float3(p.pos.x, p.radius, p.pos.z);
            }
            OD od;
            od.model  = (float4x4::CreateScale(p.radius) * rot *
                         float4x4::CreateTranslation(worldPos)).Transpose();
            od.color  = float4(1.f, 1.f, 1.f, 1.f);
            od.color2 = float4(0.f, 0.f, 0.f, 1.f);
            fbxODs.push_back(od);
        }
    }

    // shadow
    {
        const float3 kLightDir = float3(80.f, 120.f, 60.f);
        const float3 lightEye  = kLightDir * (500.f / kLightDir.Length());
        const float4x4 lightView     = float4x4::CreateLookAt(lightEye, float3::Zero, float3(0.f, 1.f, 0.f));
        const float4x4 lightProj     = float4x4::CreateOrthographic(900.f, 900.f, 1.f, 1500.f);
        const float4x4 lightViewProj = (lightView * lightProj).Transpose();

        Basic::Components::Rendering3D::SetShadowMatrix(pPipeline_, lightViewProj);

        // Unbind shadow SRV — can't be SRV and DSV at the same time
        ID3D11ShaderResourceView* nullSRV = nullptr;
        ctx->PSSetShaderResources(2, 1, &nullSRV);

        ID3D11RenderTargetView* pOldRTV = nullptr;
        ID3D11DepthStencilView* pOldDSV = nullptr;
        ctx->OMGetRenderTargets(1, &pOldRTV, &pOldDSV);

        D3D11_VIEWPORT shadowVP = {};
        shadowVP.Width    = static_cast<float>(kShadowMapSize);
        shadowVP.Height   = static_cast<float>(kShadowMapSize);
        shadowVP.MaxDepth = 1.f;
        ctx->RSSetViewports(1, &shadowVP);

        ID3D11RenderTargetView* pNullRTV = nullptr;
        ctx->OMSetRenderTargets(1, &pNullRTV, pShadowDSV_);
        ctx->ClearDepthStencilView(pShadowDSV_, D3D11_CLEAR_DEPTH, 1.f, 0);

        planeRenderer_.DrawDepthOnly({planeOD});
        ballRenderer_.DrawDepthOnly({ballOD});
        if (!sphereODs.empty()) spherePickupRenderer_.DrawDepthOnly(sphereODs);
        if (!boxODs.empty())    boxPickupRenderer_.DrawDepthOnly(boxODs);
        if (!fbxODs.empty())    fbxMeshRenderer_->DrawDepthOnly(fbxODs);

        ctx->OMSetRenderTargets(1, &pOldRTV, pOldDSV);
        if (pOldRTV) pOldRTV->Release();
        if (pOldDSV) pOldDSV->Release();
        const D3D11_VIEWPORT& vp = pPipeline_->GetViewport();
        ctx->RSSetViewports(1, &vp);
    }


    ctx->PSSetShaderResources(2, 1, &pShadowSRV_);
    ctx->PSSetSamplers(1, 1, &pShadowSampler_);

    // lighting
    {
        LD light;
        light.cameraPos        = camera_.GetEyePosition();
        light.ambientStrength  = 0.35f;
        light.specularStrength = 0.4f;
        light.shininess        = 48.f;
        light.lightPos[0]      = float4(80.f, 120.f, 60.f, 0.f);
        light.lightColor[0]    = float4(0.5f, 0.49f, 0.46f, 0.f);
        light.numLights        = 1;
        for (const auto& sl : shotLights_)
        {
            const int i         = light.numLights++;
            light.lightPos[i]   = float4(sl.pos.x, sl.pos.y, sl.pos.z, 1.f);
            light.lightColor[i] = float4(sl.color.x * 3.f, sl.color.y * 3.f, sl.color.z * 3.f, 0.f);
        }
        Basic::Components::Rendering3D::SetLight(pPipeline_, light);
    }

    // main pass
    if (!shotLights_.empty())
    {
        std::vector<OD> lightODs;
        lightODs.reserve(shotLights_.size());
        for (const auto& sl : shotLights_)
        {
            OD od;
            od.model  = (float4x4::CreateScale(1.8f) *
                         float4x4::CreateTranslation(sl.pos)).Transpose();
            od.color  = float4(sl.color.x, sl.color.y, sl.color.z, 0.9f);
            od.color2 = float4(0.f, 0.f, 0.f, 1.f);
            lightODs.push_back(od);
        }
        shotLightRenderer_.DrawInstanced(lightODs);
    }

    planeRenderer_.DrawInstanced({planeOD});
    ballRenderer_.DrawInstanced({ballOD});
    if (!sphereODs.empty()) spherePickupRenderer_.DrawInstanced(sphereODs);
    if (!boxODs.empty())    boxPickupRenderer_.DrawInstanced(boxODs);
    if (fbxMeshRenderer_ && !fbxODs.empty()) fbxMeshRenderer_->DrawInstanced(fbxODs);

    ID3D11ShaderResourceView* nullSRV = nullptr;
    ctx->PSSetShaderResources(2, 1, &nullSRV);
}


void KatamariWorld::RenderUI()
{
    ImGui::Begin("Katamari");

    ImGui::Text("Ball radius: %.2f", ballRadius_);
    ImGui::Text("Absorbed: %d / %d", absorbedCount_,
                static_cast<int>(pickups_.size() + fbxPickups_.size()));
    ImGui::Text("WASD move  |  mouse look  |  scroll zoom");

    ImGui::Separator();

    if (ImGui::Button("Reset"))
    {
        ballPos_       = float3(0, 0, 0);
        ballVel_       = float3(0, 0, 0);
        ballRadius_    = 1.5f;
        ballY_         = 0.f;
        ballVelY_      = 0.f;
        jumpsLeft_     = 2;
        rollMatrix_    = float4x4::Identity;
        absorbedCount_ = 0;
        SpawnPickups();
        fbxPickups_.clear();
    }

    ImGui::Separator();
    ImGui::Text("Import 3D mesh (FBX / OBJ / GLTF ...):");
    ImGui::InputText("##fbxpath", fbxPathBuf_, sizeof(fbxPathBuf_));
    ImGui::SameLine();
    if (ImGui::Button("Browse"))
        OpenFileBrowser();
    if (ImGui::Button("Load & Spawn"))
        LoadMesh(std::string(fbxPathBuf_));

    ImGui::End();
}

void KatamariWorld::OpenFileBrowser()
{
    char buf[MAX_PATH] = {};
    OPENFILENAMEA ofn  = {};
    ofn.lStructSize    = sizeof(ofn);
    ofn.lpstrFilter    = "3D Models\0*.fbx;*.obj;*.gltf;*.glb;*.dae;*.3ds\0All Files\0*.*\0";
    ofn.lpstrFile      = buf;
    ofn.nMaxFile       = MAX_PATH;
    ofn.Flags          = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    if (GetOpenFileNameA(&ofn))
        strncpy_s(fbxPathBuf_, buf, sizeof(fbxPathBuf_) - 1);
}

void KatamariWorld::LoadMesh(const std::string& path)
{
    if (path.empty()) return;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate | aiProcess_GenSmoothNormals |
        aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs);

    if (!scene || !scene->HasMeshes()) return;

    using Vtx = Basic::Components::Rendering3D::Vertex3D;
    std::vector<Vtx>   verts;
    std::vector<int32> indices;

    const aiMesh* mesh = scene->mMeshes[0];
    verts.reserve(mesh->mNumVertices);

    float maxExtent = 0.f;
    for (unsigned i = 0; i < mesh->mNumVertices; ++i)
    {
        const auto& v = mesh->mVertices[i];
        if (fabsf(v.x) > maxExtent) maxExtent = fabsf(v.x);
        if (fabsf(v.y) > maxExtent) maxExtent = fabsf(v.y);
        if (fabsf(v.z) > maxExtent) maxExtent = fabsf(v.z);
    }

    const float s = (maxExtent > 0.f) ? 1.f / maxExtent : 1.f;
    for (unsigned i = 0; i < mesh->mNumVertices; ++i)
    {
        Vtx v;
        v.position = float3(mesh->mVertices[i].x * s,
                            mesh->mVertices[i].y * s,
                            mesh->mVertices[i].z * s);
        v.normal   = mesh->HasNormals()
                   ? float3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)
                   : float3(0.f, 1.f, 0.f);
        v.uv       = mesh->HasTextureCoords(0)
                   ? float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)
                   : float2(0.f, 0.f);
        verts.push_back(v);
    }

    for (unsigned i = 0; i < mesh->mNumFaces; ++i)
    {
        const aiFace& face = mesh->mFaces[i];
        for (unsigned j = 0; j < face.mNumIndices; ++j)
            indices.push_back(static_cast<int32>(face.mIndices[j]));
    }

    if (verts.empty() || indices.empty()) return;


    namespace fs = std::filesystem;
    const fs::path meshPath(path);
    const std::string texPath = (meshPath.parent_path() / ("tex_" + meshPath.stem().string() + ".png")).string();


    ID3D11ShaderResourceView* pSRV = nullptr;
    {
        std::wstring wpath(texPath.begin(), texPath.end());
        DirectX::CreateWICTextureFromFile(pPipeline_->GetDevice(), nullptr,
                                          wpath.c_str(), nullptr, &pSRV);
    }
    if (!pSRV)
    {
        constexpr uint32 white = 0xFFFFFFFF;
        D3D11_TEXTURE2D_DESC td = {};
        td.Width = td.Height = 1;
        td.MipLevels = td.ArraySize = 1;
        td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        td.SampleDesc.Count = 1;
        td.Usage = D3D11_USAGE_DEFAULT;
        td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        D3D11_SUBRESOURCE_DATA td_data = { &white, 4, 0 };
        ID3D11Texture2D* pTex = nullptr;
        pPipeline_->GetDevice()->CreateTexture2D(&td, &td_data, &pTex);
        if (pTex)
        {
            pPipeline_->GetDevice()->CreateShaderResourceView(pTex, nullptr, &pSRV);
            pTex->Release();
        }
    }

    if (fbxTexSRV_) { fbxTexSRV_->Release(); fbxTexSRV_ = nullptr; }
    fbxTexSRV_ = pSRV;

    fbxMeshRenderer_ = std::make_unique<Basic::Components::Rendering3D>();
    fbxMeshRenderer_->Construct(pPipeline_, verts, indices,
        Basic::Components::Rendering3D::ShaderType::PhongTex, fbxTexSRV_);


    // extract positions only for collision
    std::vector<float3> colVerts;
    colVerts.reserve(verts.size());
    for (const auto& v : verts) colVerts.push_back(v.position);

    const unsigned seed = static_cast<unsigned>(reinterpret_cast<uintptr_t>(fbxTexSRV_) & 0xFFFF);
    srand(seed ? seed : 1u);
    for (int i = 0; i < 5; ++i)
    {
        FbxPickup p;
        p.pos.x      = (rand() % 6000 - 3000) / 100.f;
        p.pos.z      = (rand() % 6000 - 3000) / 100.f;
        p.radius     = (0.7f + (rand() % 120) / 100.f) * 2.f;
        p.meshVerts   = colVerts;
        p.meshIndices = indices;
        fbxPickups_.push_back(p);
    }
}
