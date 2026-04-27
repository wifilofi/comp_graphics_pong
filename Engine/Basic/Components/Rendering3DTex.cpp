#include "Rendering3DTex.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include "../../Render/Pipeline.h"

using namespace Basic::Components;

void Rendering3DTex::Construct(Engine::Render::Pipeline* pPipeline,
                               const std::vector<Vertex3DTex>& vertices,
                               const std::vector<int32>& indices,
                               ID3D11ShaderResourceView* pTextureSRV)
{
    pPipeline_   = pPipeline;
    pTextureSRV_ = pTextureSRV;
    indexCount_  = static_cast<int32>(indices.size());
    auto* device = pPipeline_->GetDevice();

    static DXVertexShader* s_pVS     = nullptr;
    static DXPixelShader*  s_pPS     = nullptr;
    static DXInputLayout*  s_pLayout = nullptr;

    if (!s_pVS)
    {
        DXBlob* pVSBlob = nullptr;
        DXBlob* pPSBlob = nullptr;
        DXBlob* pError  = nullptr;

        D3DCompileFromFile(L"././Shaders/ShaderTex3D.hlsl", nullptr, nullptr,
                           "VSMain", "vs_5_0",
                           D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
                           0, &pVSBlob, &pError);
        if (pError) { pError->Release(); pError = nullptr; }
        if (!pVSBlob) return;

        D3DCompileFromFile(L"././Shaders/ShaderTex3D.hlsl", nullptr, nullptr,
                           "PSMain", "ps_5_0",
                           D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
                           0, &pPSBlob, &pError);
        if (pError) { pError->Release(); pError = nullptr; }
        if (!pPSBlob) { pVSBlob->Release(); return; }

        device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(),
                                   nullptr, &s_pVS);
        device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(),
                                  nullptr, &s_pPS);

        const D3D11_INPUT_ELEMENT_DESC layout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        device->CreateInputLayout(layout, 3, pVSBlob->GetBufferPointer(),
                                  pVSBlob->GetBufferSize(), &s_pLayout);
        pVSBlob->Release();
        pPSBlob->Release();
    }

    if (!s_pVS || !s_pPS || !s_pLayout) return;

    pVertexShader_ = s_pVS;
    pPixelShader_  = s_pPS;
    pInputLayout_  = s_pLayout;

    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage     = D3D11_USAGE_DEFAULT;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex3DTex) * vertices.size());
    D3D11_SUBRESOURCE_DATA vbData = { vertices.data(), 0, 0 };
    device->CreateBuffer(&vbDesc, &vbData, &pVertexBuffer_);

    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.Usage     = D3D11_USAGE_DEFAULT;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibDesc.ByteWidth = static_cast<UINT>(sizeof(int32) * indices.size());
    D3D11_SUBRESOURCE_DATA ibData = { indices.data(), 0, 0 };
    device->CreateBuffer(&ibDesc, &ibData, &pIndexBuffer_);

    if (!pVertexBuffer_ || !pIndexBuffer_) return;

    static DXRasterizerState* s_pRast = nullptr;
    if (!s_pRast)
    {
        CD3D11_RASTERIZER_DESC desc(D3D11_DEFAULT);
        desc.CullMode = D3D11_CULL_NONE;
        device->CreateRasterizerState(&desc, &s_pRast);
    }
    pRasterizerState_ = s_pRast;

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

    valid_ = true;
}

void Rendering3DTex::EnsureInstanceBuffer(int count)
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
    srvDesc.Format              = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension       = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements  = static_cast<UINT>(instanceCapacity_);
    device->CreateShaderResourceView(pInstanceBuffer_, &srvDesc, &pInstanceSRV_);
}

void Rendering3DTex::DrawInstanced(const std::vector<ObjectData>& instances)
{
    if (!valid_ || instances.empty()) return;

    const int count = static_cast<int>(instances.size());
    EnsureInstanceBuffer(count);

    auto* ctx = pPipeline_->GetDeviceContext();

    D3D11_MAPPED_SUBRESOURCE sub = {};
    ctx->Map(pInstanceBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub);
    memcpy(sub.pData, instances.data(), sizeof(ObjectData) * count);
    ctx->Unmap(pInstanceBuffer_, 0);

    ctx->VSSetShaderResources(0, 1, &pInstanceSRV_);
    ctx->PSSetShaderResources(1, 1, &pTextureSRV_);
    ctx->PSSetSamplers(0, 1, &pSamplerState_);

    constexpr UINT stride = sizeof(Vertex3DTex);
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
