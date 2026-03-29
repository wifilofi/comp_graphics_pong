#include "Rendering.h"

#include <d3dcompiler.h>

#include "../../Render/Pipeline.h"

using namespace Basic::Components;

void Rendering::Compose(Engine::Render::Pipeline* pPipeline)
{
    pPipeline_ = pPipeline;
}

void Rendering::Render(float delta)
{
    auto* context = pPipeline_->GetDeviceContext();
    context->RSSetState(pRasterizerState_);

    constexpr uint32 strides[] = {32};
    constexpr uint32 offsets[] = {0};

    context->IASetInputLayout(pInputLayout_);
    context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R32_UINT, 0);
    context->IASetVertexBuffers(0, 1, &pVertexBuffer_, strides, offsets);
    context->VSSetShader(pVertexShader_, nullptr, 0);
    context->PSSetShader(pPixelShader_, nullptr, 0);

    context->DrawIndexed(indicesAmount_, 0, 0);
}

void Rendering::CreateVertexShader(
    const char16* pPath, const DXShaderMacros* pDefines, DXIncludes* pIncludes)
{
    DXBlob* pError;
    D3DCompileFromFile(
        pPath, pDefines, pIncludes, "VSMain", "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pVertexShaderByteCode_, &pError);
    pPipeline_->GetDevice()->CreateVertexShader(
        pVertexShaderByteCode_->GetBufferPointer(), pVertexShaderByteCode_->GetBufferSize(),
        nullptr, &pVertexShader_);
}

void Rendering::CreatePixelShader(
    const char16* pPath, const DXShaderMacros* pDefines, DXIncludes* pIncludes)
{
    DXBlob* pError;
    D3DCompileFromFile(
        pPath, pDefines, pIncludes, "PSMain", "ps_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pPixelShaderByteCode_, &pError);
    pPipeline_->GetDevice()->CreatePixelShader(
        pPixelShaderByteCode_->GetBufferPointer(), pPixelShaderByteCode_->GetBufferSize(),
        nullptr, &pPixelShader_);
}

void Rendering::CreateLayout()
{
    constexpr D3D11_INPUT_ELEMENT_DESC inputElementsDescriptor[] = {
        D3D11_INPUT_ELEMENT_DESC{
            "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
            0, D3D11_INPUT_PER_VERTEX_DATA, 0
        },
        D3D11_INPUT_ELEMENT_DESC{
            "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
        }
    };

    pPipeline_->GetDevice()->CreateInputLayout(
        inputElementsDescriptor, 2, pVertexShaderByteCode_->GetBufferPointer(),
        pVertexShaderByteCode_->GetBufferSize(), &pInputLayout_);
}

void Rendering::CreateVertexBuffer(uint32 pointsAmount)
{
    D3D11_BUFFER_DESC vertexBufferDescriptor;
    vertexBufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDescriptor.CPUAccessFlags = 0;
    vertexBufferDescriptor.MiscFlags = 0;
    vertexBufferDescriptor.StructureByteStride = 0;
    vertexBufferDescriptor.ByteWidth = sizeof(float4) * pointsAmount;

    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = pPoints_;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    pPipeline_->GetDevice()->CreateBuffer(&vertexBufferDescriptor, &vertexData, &pVertexBuffer_);
}

void Rendering::CreateIndexBuffer(uint32 indicesAmount)
{
    D3D11_BUFFER_DESC indexBufferDescriptor;
    indexBufferDescriptor.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDescriptor.CPUAccessFlags = 0;
    indexBufferDescriptor.MiscFlags = 0;
    indexBufferDescriptor.StructureByteStride = 0;
    indexBufferDescriptor.ByteWidth = sizeof(int32) * indicesAmount;

    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = pIndices_;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    pPipeline_->GetDevice()->CreateBuffer(&indexBufferDescriptor, &indexData, &pIndexBuffer_);
}

void Rendering::CreateRasterizerState()
{
    CD3D11_RASTERIZER_DESC rasterizerDescriptor = {};
    rasterizerDescriptor.CullMode = D3D11_CULL_NONE;
    rasterizerDescriptor.FillMode = D3D11_FILL_SOLID;

    pPipeline_->GetDevice()->CreateRasterizerState(&rasterizerDescriptor, &pRasterizerState_);
}
