#include "Rendering3D.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include "../../Render/Pipeline.h"

using namespace Basic::Components;

void Rendering3D::Construct(Engine::Render::Pipeline* pPipeline,
                            const std::vector<Vertex3D>& vertices,
                            const std::vector<int32>& indices)
{
    pPipeline_  = pPipeline;
    indexCount_ = static_cast<int32>(indices.size());
    auto* device = pPipeline_->GetDevice();

    // Shared across all Rendering3D instances — only compiled once.
    static DXVertexShader*    s_pVertexShader  = nullptr;
    static DXPixelShader*     s_pPixelShader   = nullptr;
    static DXInputLayout*     s_pInputLayout   = nullptr;

    if (!s_pVertexShader)
    {
        DXBlob* pVSBlob = nullptr;
        DXBlob* pError  = nullptr;
        D3DCompileFromFile(L"././Shaders/Shader3D.hlsl", nullptr, nullptr,
                           "VSMain", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
                           0, &pVSBlob, &pError);
        device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(),
                                   nullptr, &s_pVertexShader);

        DXBlob* pPSBlob = nullptr;
        D3DCompileFromFile(L"././Shaders/Shader3D.hlsl", nullptr, nullptr,
                           "PSMain", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
                           0, &pPSBlob, &pError);
        device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(),
                                  nullptr, &s_pPixelShader);

        const D3D11_INPUT_ELEMENT_DESC layout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        device->CreateInputLayout(layout, 2, pVSBlob->GetBufferPointer(),
                                  pVSBlob->GetBufferSize(), &s_pInputLayout);
        pVSBlob->Release();
        if (pPSBlob) pPSBlob->Release();
    }

    pVertexShader_  = s_pVertexShader;
    pPixelShader_   = s_pPixelShader;
    pInputLayout_   = s_pInputLayout;

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

    D3D11_BUFFER_DESC obDesc = {};
    obDesc.Usage          = D3D11_USAGE_DYNAMIC;
    obDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    obDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    obDesc.ByteWidth      = sizeof(ObjectData);
    device->CreateBuffer(&obDesc, nullptr, &pObjectBuffer_);

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

void Rendering3D::Draw(const float4x4& model, const float4& color)
{
    auto* ctx = pPipeline_->GetDeviceContext();

    D3D11_MAPPED_SUBRESOURCE sub = {};
    ctx->Map(pObjectBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub);
    auto* pObj    = static_cast<ObjectData*>(sub.pData);
    pObj->model   = model.Transpose();
    pObj->color   = color;
    ctx->Unmap(pObjectBuffer_, 0);
    ctx->VSSetConstantBuffers(0, 1, &pObjectBuffer_);
    ctx->PSSetConstantBuffers(0, 1, &pObjectBuffer_);

    constexpr UINT stride = sizeof(Vertex3D);
    constexpr UINT offset = 0;
    ctx->RSSetState(pRasterizerState_);
    ctx->IASetInputLayout(pInputLayout_);
    ctx->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ctx->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);
    ctx->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R32_UINT, 0);
    ctx->VSSetShader(pVertexShader_, nullptr, 0);
    ctx->PSSetShader(pPixelShader_, nullptr, 0);
    ctx->DrawIndexed(indexCount_, 0, 0);
}
