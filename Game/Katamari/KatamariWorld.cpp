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
static constexpr float kJumpForce    = 0.4f;
static constexpr float kGravityRise  = 0.012f;
static constexpr float kGravityFall  = 0.030f;


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
        const std::wstring texPath = (fs::path(__FILE__).parent_path() / "Textures" / "T_player_eshka.png").wstring();
        DirectX::CreateWICTextureFromFile(pPipeline_->GetDevice(), nullptr, texPath.c_str(), nullptr, &ballTexSRV_);
    }
    ballRenderer_.Construct(pPipeline_, sphereV, sphereI, ST::ShaderTex, ballTexSRV_);
    ballRenderer_.SetLight({ {0.4f,-1.f,0.6f}, 0.f, {1.f,0.95f,0.85f}, 0.f, {0.15f,0.15f,0.2f}, 0.f });
    spherePickupRenderer_.Construct(pPipeline_, sphereV, sphereI, ST::PerlinNoise);
    planeRenderer_       .Construct(pPipeline_, boxV,    boxI,    ST::SolidColor);
    boxPickupRenderer_   .Construct(pPipeline_, boxV,    boxI,    ST::PerlinNoise);

    SpawnPickups();
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

// ---- FixedUpdate -------------------------------------------------------------

void KatamariWorld::FixedUpdate()
{
    UpdateBall();
    CheckCollisions();
    camera_.SetTarget(float3(ballPos_.x, ballRadius_ + ballY_, ballPos_.z));
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

// ---- Render ------------------------------------------------------------------

void KatamariWorld::Render(float /*delta*/)
{
    using OD = Basic::Components::Rendering3D::ObjectData;

    // Plane
    {
        OD od;
        od.model  = (float4x4::CreateScale(kPlaneHalf, 0.2f, kPlaneHalf) *
                     float4x4::CreateTranslation(0.f, -0.1f, 0.f)).Transpose();
        od.color  = float4(0.35f, 0.55f, 0.25f, 1);
        od.color2 = float4(0.2f,  0.35f, 0.15f, 1);
        planeRenderer_.DrawInstanced({od});
    }

    // Ball
    {
        const float3 ballCenter(ballPos_.x, ballRadius_ + ballY_, ballPos_.z);
        OD od;
        od.model  = (float4x4::CreateScale(ballRadius_) *
                     rollMatrix_ *
                     float4x4::CreateTranslation(ballCenter)).Transpose();
        od.color  = float4(0.92f, 0.87f, 0.82f, 1);
        od.color2 = float4(0.45f, 0.32f, 0.20f, 1);
        ballRenderer_.DrawInstanced({od});
    }

    // Pickups (absorbed + free)
    const float3 ballCenter(ballPos_.x, ballRadius_ + ballY_, ballPos_.z);
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

    if (!sphereODs.empty()) spherePickupRenderer_.DrawInstanced(sphereODs);
    if (!boxODs.empty())    boxPickupRenderer_.DrawInstanced(boxODs);

    // FBX pickups
    if (fbxMeshRenderer_ && !fbxPickups_.empty())
    {
        std::vector<OD> fbxODs;
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
        fbxMeshRenderer_->DrawInstanced(fbxODs);
    }
}

// ---- UI ----------------------------------------------------------------------

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
        Basic::Components::Rendering3D::ShaderType::ShaderTex, fbxTexSRV_);


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
