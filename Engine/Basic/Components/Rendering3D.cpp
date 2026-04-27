#include "Rendering3D.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include "../../Render/Pipeline.h"

using namespace Basic::Components;

static const char kShaderTex3D[] = R"hlsl(
struct ObjectData { float4x4 model; float4 color; float4 color2; };
StructuredBuffer<ObjectData> instances : register(t0);
Texture2D    mainTex  : register(t1);
SamplerState sampler0 : register(s0);
cbuffer CameraBuffer : register(b2) { float4x4 view; float4x4 projection; };
struct VS_IN  { float3 pos:POSITION0; float3 normal:NORMAL0; float2 uv:TEXCOORD0; };
struct PS_IN  { float4 pos:SV_POSITION; float2 uv:TEXCOORD0; float4 color:COLOR0; };
PS_IN VSMain(VS_IN i, uint id:SV_InstanceID) {
    ObjectData o = instances[id]; PS_IN r;
    r.pos   = mul(mul(float4(i.pos,1), o.model), mul(view, projection));
    r.uv    = i.uv; r.color = o.color; return r; }
float4 PSMain(PS_IN i):SV_Target { return mainTex.Sample(sampler0,i.uv)*i.color; }
)hlsl";

void Rendering3D::Construct(Engine::Render::Pipeline* pPipeline,
                            const std::vector<Vertex3D>& vertices,
                            const std::vector<int32>& indices,
                            ShaderType shaderType,
                            ID3D11ShaderResourceView* pTextureSRV)
{
    pPipeline_   = pPipeline;
    shaderType_  = shaderType;
    pTextureSRV_ = pTextureSRV;
    indexCount_  = static_cast<int32>(indices.size());
    auto* device = pPipeline_->GetDevice();

    static DXVertexShader* s_pVS[3]     = {};
    static DXPixelShader*  s_pPS[3]     = {};
    static DXInputLayout*  s_pLayout[3] = {};

    const int idx = static_cast<int>(shaderType);

    if (!s_pVS[idx])
    {
        DXBlob* pVSBlob = nullptr;
        DXBlob* pPSBlob = nullptr;
        DXBlob* pError  = nullptr;

        if (shaderType == ShaderType::ShaderTex)
        {
            D3DCompile(kShaderTex3D, sizeof(kShaderTex3D) - 1, nullptr, nullptr, nullptr,
                       "VSMain", "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pVSBlob, &pError);
            if (pError) { pError->Release(); pError = nullptr; }
            if (!pVSBlob) return;

            D3DCompile(kShaderTex3D, sizeof(kShaderTex3D) - 1, nullptr, nullptr, nullptr,
                       "PSMain", "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pPSBlob, &pError);
            if (pError) { pError->Release(); pError = nullptr; }
        }
        else
        {
            const wchar_t* file = (shaderType == ShaderType::PerlinNoise)
                ? L"././Shaders/ShaderNoise3D.hlsl"
                : L"././Shaders/Shader3D.hlsl";

            D3DCompileFromFile(file, nullptr, nullptr,
                               "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
                               0, &pVSBlob, &pError);
            if (pError) { pError->Release(); pError = nullptr; }
            if (!pVSBlob) return;

            D3DCompileFromFile(file, nullptr, nullptr,
                               "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
                               0, &pPSBlob, &pError);
            if (pError) { pError->Release(); pError = nullptr; }
        }

        if (!pVSBlob) return;
        device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(),
                                   nullptr, &s_pVS[idx]);
        if (pPSBlob)
        {
            device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(),
                                      nullptr, &s_pPS[idx]);
            pPSBlob->Release();
        }

        if (shaderType == ShaderType::ShaderTex)
        {
            const D3D11_INPUT_ELEMENT_DESC layout[] = {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            };
            device->CreateInputLayout(layout, 3, pVSBlob->GetBufferPointer(),
                                      pVSBlob->GetBufferSize(), &s_pLayout[idx]);
        }
        else
        {
            const D3D11_INPUT_ELEMENT_DESC layout[] = {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            };
            device->CreateInputLayout(layout, 2, pVSBlob->GetBufferPointer(),
                                      pVSBlob->GetBufferSize(), &s_pLayout[idx]);
        }

        pVSBlob->Release();
    }

    if (!s_pVS[idx] || !s_pPS[idx] || !s_pLayout[idx]) return;

    pVertexShader_ = s_pVS[idx];
    pPixelShader_  = s_pPS[idx];
    pInputLayout_  = s_pLayout[idx];

    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage     = D3D11_USAGE_DEFAULT;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex3D) * vertices.size());
    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = vertices.data();
    device->CreateBuffer(&vbDesc, &vbData, &pVertexBuffer_);

    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.Usage     = D3D11_USAGE_DEFAULT;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibDesc.ByteWidth = static_cast<UINT>(sizeof(int32) * indices.size());
    D3D11_SUBRESOURCE_DATA ibData = {};
    ibData.pSysMem = indices.data();
    device->CreateBuffer(&ibDesc, &ibData, &pIndexBuffer_);

    static DXRasterizerState* s_pRastCull   = nullptr;
    static DXRasterizerState* s_pRastNoCull = nullptr;

    if (!s_pRastCull)
    {
        CD3D11_RASTERIZER_DESC desc(D3D11_DEFAULT);
        desc.CullMode = D3D11_CULL_BACK;
        device->CreateRasterizerState(&desc, &s_pRastCull);
    }
    if (!s_pRastNoCull)
    {
        CD3D11_RASTERIZER_DESC desc(D3D11_DEFAULT);
        desc.CullMode = D3D11_CULL_NONE;
        device->CreateRasterizerState(&desc, &s_pRastNoCull);
    }
    pRasterizerState_ = (shaderType == ShaderType::ShaderTex) ? s_pRastNoCull : s_pRastCull;

    if (shaderType == ShaderType::ShaderTex)
    {
        static ID3D11SamplerState* s_pSampler = nullptr;
        if (!s_pSampler)
        {
            D3D11_SAMPLER_DESC sd = {};
            sd.Filter   = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
            sd.MaxLOD   = D3D11_FLOAT32_MAX;
            device->CreateSamplerState(&sd, &s_pSampler);
        }
        pSamplerState_ = s_pSampler;
    }
}

void Rendering3D::EnsureInstanceBuffer(int count)
{
    if (count <= instanceCapacity_) return;

    if (pInstanceSRV_)    { pInstanceSRV_->Release();    pInstanceSRV_    = nullptr; }
    if (pInstanceBuffer_) { pInstanceBuffer_->Release(); pInstanceBuffer_ = nullptr; }

    instanceCapacity_ = count;
    auto* device = pPipeline_->GetDevice();

    D3D11_BUFFER_DESC desc = {};
    desc.Usage               = D3D11_USAGE_DYNAMIC;
    desc.BindFlags           = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    desc.StructureByteStride = sizeof(ObjectData);
    desc.ByteWidth           = sizeof(ObjectData) * static_cast<UINT>(instanceCapacity_);
    device->CreateBuffer(&desc, nullptr, &pInstanceBuffer_);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format                  = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension           = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement     = 0;
    srvDesc.Buffer.NumElements      = static_cast<UINT>(instanceCapacity_);
    device->CreateShaderResourceView(pInstanceBuffer_, &srvDesc, &pInstanceSRV_);
}

void Rendering3D::DrawInstanced(const std::vector<ObjectData>& instances)
{
    if (!pVertexShader_ || instances.empty()) return;

    const int count = static_cast<int>(instances.size());
    EnsureInstanceBuffer(count);

    auto* ctx = pPipeline_->GetDeviceContext();

    D3D11_MAPPED_SUBRESOURCE sub = {};
    ctx->Map(pInstanceBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub);
    memcpy(sub.pData, instances.data(), sizeof(ObjectData) * count);
    ctx->Unmap(pInstanceBuffer_, 0);

    ctx->VSSetShaderResources(0, 1, &pInstanceSRV_);

    if (shaderType_ == ShaderType::ShaderTex)
    {
        ctx->PSSetShaderResources(1, 1, &pTextureSRV_);
        ctx->PSSetSamplers(0, 1, &pSamplerState_);
    }

    constexpr UINT stride = sizeof(Vertex3D);
    constexpr UINT offset = 0;
    ctx->RSSetState(pRasterizerState_);
    ctx->IASetInputLayout(pInputLayout_);
    ctx->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ctx->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);
    ctx->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R32_UINT, 0);
    ctx->VSSetShader(pVertexShader_, nullptr, 0);
    ctx->PSSetShader(pPixelShader_, nullptr, 0);
    ctx->DrawIndexedInstanced(static_cast<UINT>(indexCount_), static_cast<UINT>(count), 0, 0, 0);
}
