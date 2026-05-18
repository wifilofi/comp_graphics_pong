#include "PostProcess.h"
#include "Pipeline.h"
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

using namespace Engine::Render;

static const char kFullscreenVS[] = R"hlsl(
struct PS_IN { float4 pos : SV_POSITION; float2 uv : TEXCOORD0; };
PS_IN VSMain(uint id : SV_VertexID)
{
    float2 uv  = float2((id & 1) * 2.0, (id >> 1) * 2.0);
    PS_IN  o;
    o.pos = float4(uv.x * 2.0 - 1.0, 1.0 - uv.y * 2.0, 0.0, 1.0);
    o.uv  = uv;
    return o;
}
)hlsl";

void PostProcess::Construct(Pipeline* pPipeline, int width, int height)
{
    pPipeline_ = pPipeline;
    width_     = width;
    height_    = height;
    CreateTextures(width, height);
    EnsureInfra();
}

void PostProcess::EnsureInfra()
{
    if (pFullscreenVS_) return;

    auto* device = pPipeline_->GetDevice();

    DXBlob* pVSBlob = nullptr;
    DXBlob* pError  = nullptr;
    D3DCompile(kFullscreenVS, sizeof(kFullscreenVS) - 1, nullptr, nullptr, nullptr,
               "VSMain", "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pVSBlob, &pError);
    if (pError)  { pError->Release();  pError  = nullptr; }
    if (pVSBlob)
    {
        device->CreateVertexShader(pVSBlob->GetBufferPointer(),
                                   pVSBlob->GetBufferSize(), nullptr, &pFullscreenVS_);
        pVSBlob->Release();
    }

    D3D11_SAMPLER_DESC sd = {};
    sd.Filter   = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.MaxLOD   = D3D11_FLOAT32_MAX;
    device->CreateSamplerState(&sd, &pSampler_);

    D3D11_DEPTH_STENCIL_DESC dsd = {};
    dsd.DepthEnable = FALSE;
    device->CreateDepthStencilState(&dsd, &pNoDepthState_);
}

void PostProcess::CreateTextures(int w, int h)
{
    auto* device = pPipeline_->GetDevice();

    for (int i = 0; i < 2; ++i)
    {
        D3D11_TEXTURE2D_DESC td = {};
        td.Width            = static_cast<UINT>(w);
        td.Height           = static_cast<UINT>(h);
        td.MipLevels        = 1;
        td.ArraySize        = 1;
        td.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
        td.SampleDesc.Count = 1;
        td.Usage            = D3D11_USAGE_DEFAULT;
        td.BindFlags        = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        device->CreateTexture2D(&td, nullptr, &pTex_[i]);
        device->CreateRenderTargetView(pTex_[i],  nullptr, &pSceneRTV_[i]);
        device->CreateShaderResourceView(pTex_[i], nullptr, &pSceneSRV_[i]);
    }

    D3D11_TEXTURE2D_DESC dd = {};
    dd.Width            = static_cast<UINT>(w);
    dd.Height           = static_cast<UINT>(h);
    dd.MipLevels        = 1;
    dd.ArraySize        = 1;
    dd.Format           = DXGI_FORMAT_D32_FLOAT;
    dd.SampleDesc.Count = 1;
    dd.Usage            = D3D11_USAGE_DEFAULT;
    dd.BindFlags        = D3D11_BIND_DEPTH_STENCIL;
    device->CreateTexture2D(&dd, nullptr, &pDepthTex_);
    device->CreateDepthStencilView(pDepthTex_, nullptr, &pSceneDSV_);
}

void PostProcess::DestroyTextures()
{
    for (int i = 0; i < 2; ++i)
    {
        if (pSceneSRV_[i]) { pSceneSRV_[i]->Release(); pSceneSRV_[i] = nullptr; }
        if (pSceneRTV_[i]) { pSceneRTV_[i]->Release(); pSceneRTV_[i] = nullptr; }
        if (pTex_[i])      { pTex_[i]->Release();      pTex_[i]      = nullptr; }
    }
    if (pSceneDSV_) { pSceneDSV_->Release(); pSceneDSV_ = nullptr; }
    if (pDepthTex_) { pDepthTex_->Release(); pDepthTex_ = nullptr; }
}

void PostProcess::Resize(int w, int h)
{
    width_  = w;
    height_ = h;
    DestroyTextures();
    CreateTextures(w, h);
}

void PostProcess::AddPass(const wchar_t* psFile)
{
    EnsureInfra();

    DXBlob* pPSBlob = nullptr;
    DXBlob* pError  = nullptr;
    D3DCompileFromFile(psFile, nullptr, nullptr,
                       "PSMain", "ps_5_0",
                       D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
                       0, &pPSBlob, &pError);
    if (pError)  { pError->Release();  return; }
    if (!pPSBlob) return;

    Pass pass;
    pPipeline_->GetDevice()->CreatePixelShader(
        pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &pass.pPS);
    pPSBlob->Release();

    if (pass.pPS)
        passes_.push_back(pass);
}

void PostProcess::Apply(ID3D11RenderTargetView* pBackBuffer)
{
    if (passes_.empty() || !pFullscreenVS_) return;

    auto* ctx = pPipeline_->GetDeviceContext();

    D3D11_VIEWPORT vp = {};
    vp.Width    = static_cast<float>(width_);
    vp.Height   = static_cast<float>(height_);
    vp.MaxDepth = 1.f;
    ctx->RSSetViewports(1, &vp);

    ctx->OMSetDepthStencilState(pNoDepthState_, 0);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ctx->IASetInputLayout(nullptr);
    ctx->VSSetShader(pFullscreenVS_, nullptr, 0);
    ctx->PSSetSamplers(0, 1, &pSampler_);

    int src = 0;
    for (size_t i = 0; i < passes_.size(); ++i)
    {
        int dst = 1 - src;
        ID3D11RenderTargetView* outRTV = (i == passes_.size() - 1) ? pBackBuffer : pSceneRTV_[dst];

        ctx->OMSetRenderTargets(1, &outRTV, nullptr);
        ctx->PSSetShaderResources(0, 1, &pSceneSRV_[src]);
        ctx->PSSetShader(passes_[i].pPS, nullptr, 0);
        ctx->Draw(3, 0);

        ID3D11ShaderResourceView* nullSRV = nullptr;
        ctx->PSSetShaderResources(0, 1, &nullSRV);
        src = dst;
    }

    ctx->OMSetDepthStencilState(nullptr, 0);
}
