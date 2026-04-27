#include "Rendering3D.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include "../../Render/Pipeline.h"

using namespace Basic::Components;

void Rendering3D::Construct(Engine::Render::Pipeline* pPipeline,
                            const std::vector<Vertex3D>& vertices,
                            const std::vector<int32>& indices,
                            ShaderType shaderType)
{
    pPipeline_  = pPipeline;
    indexCount_ = static_cast<int32>(indices.size());
    auto* device = pPipeline_->GetDevice();

    static DXVertexShader* s_pVS[2]     = {};
    static DXPixelShader*  s_pPS[2]     = {};
    static DXInputLayout*  s_pLayout[2] = {};

    const int idx = static_cast<int>(shaderType);

    if (!s_pVS[idx])
    {
        const wchar_t* file = (shaderType == ShaderType::PerlinNoise)
            ? L"././Shaders/ShaderNoise3D.hlsl"
            : L"././Shaders/ShaderTex3D.hlsl";

        DXBlob* pVSBlob = nullptr;
        DXBlob* pError  = nullptr;
        D3DCompileFromFile(file, nullptr, nullptr,
                           "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
                           0, &pVSBlob, &pError);
        device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(),
                                   nullptr, &s_pVS[idx]);

        DXBlob* pPSBlob = nullptr;
        D3DCompileFromFile(file, nullptr, nullptr,
                           "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
                           0, &pPSBlob, &pError);
        device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(),
                                  nullptr, &s_pPS[idx]);

        const D3D11_INPUT_ELEMENT_DESC layout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        device->CreateInputLayout(layout, 2, pVSBlob->GetBufferPointer(),
                                  pVSBlob->GetBufferSize(), &s_pLayout[idx]);
        pVSBlob->Release();
        if (pPSBlob) pPSBlob->Release();
    }

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

    static DXRasterizerState* s_pRasterizerState = nullptr;
    if (!s_pRasterizerState)
    {
        CD3D11_RASTERIZER_DESC rastDesc = {};
        rastDesc.FillMode = D3D11_FILL_SOLID;
        rastDesc.CullMode = D3D11_CULL_BACK;
        rastDesc.FrontCounterClockwise = FALSE;
        device->CreateRasterizerState(&rastDesc, &s_pRasterizerState);
    }
    pRasterizerState_ = s_pRasterizerState;
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
    if (instances.empty()) return;

    const int count = static_cast<int>(instances.size());
    EnsureInstanceBuffer(count);

    auto* ctx = pPipeline_->GetDeviceContext();

    D3D11_MAPPED_SUBRESOURCE sub = {};
    ctx->Map(pInstanceBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub);
    memcpy(sub.pData, instances.data(), sizeof(ObjectData) * count);
    ctx->Unmap(pInstanceBuffer_, 0);

    ctx->VSSetShaderResources(0, 1, &pInstanceSRV_);

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
