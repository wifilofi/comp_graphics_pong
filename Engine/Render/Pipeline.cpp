#include "Pipeline.h"
#include <d3d11_1.h>

#pragma comment(lib, "d3d11.lib")

using namespace Engine::Render;

void Pipeline::Construct(PHandlerWindow pHandlerWindow, const Point& size)
{
    hwnd_ = pHandlerWindow;
    size_ = size;
    gameSize_ = size;
    viewport_.Width = static_cast<float>(size.x);
    viewport_.Height = static_cast<float>(size.y);
    viewport_.TopLeftX = 0;
    viewport_.TopLeftY = 0;
    viewport_.MinDepth = 0;
    viewport_.MaxDepth = 1.0f;
    ConstructDeviceAndSwapChain(pHandlerWindow);
    ConstructRenderTargetView();
    ConstructBlendState();
}
void Pipeline::Render(float delta) const
{
    pDeviceContext_->ClearState();
    pDeviceContext_->OMSetRenderTargets(1, &pRenderTargetView_, nullptr);

    // Black for the letterbox/pillarbox bars
    constexpr float black[] = {0.0f, 0.0f, 0.0f, 1.0f};
    pDeviceContext_->ClearRenderTargetView(pRenderTargetView_, black);

    // #2c4b76 for the game area only
    ID3D11DeviceContext1* pContext1 = nullptr;
    pDeviceContext_->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&pContext1));
    if (pContext1)
    {
        constexpr float gameColor[] = {0x2c / 255.0f, 0x4b / 255.0f, 0x76 / 255.0f, 1.0f};
        const D3D11_RECT rect = {
            static_cast<LONG>(viewport_.TopLeftX),
            static_cast<LONG>(viewport_.TopLeftY),
            static_cast<LONG>(viewport_.TopLeftX + viewport_.Width),
            static_cast<LONG>(viewport_.TopLeftY + viewport_.Height)
        };
        pContext1->ClearView(pRenderTargetView_, gameColor, &rect, 1);
        pContext1->Release();
    }

    constexpr float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
    pDeviceContext_->OMSetBlendState(pBlendState_, blendFactor, 0xFFFFFFFF);
    pDeviceContext_->RSSetViewports(1, &viewport_);
    for (auto* pRenderAble : renderAbles_)
    {
        pDeviceContext_->OMSetRenderTargets(1, &pRenderTargetView_, nullptr);
        pRenderAble->Render(delta);
    }
    pSwapChain_->Present(1, 0);
}

void Pipeline::Resize(int newWidth, int newHeight)
{
    if (!pSwapChain_) return;

    pDeviceContext_->OMSetRenderTargets(0, nullptr, nullptr);
    pDeviceContext_->ClearState();
    pDeviceContext_->Flush();

    pRenderTargetView_->Release();
    pRenderTargetView_ = nullptr;

    pSwapChain_->ResizeBuffers(0, newWidth, newHeight, DXGI_FORMAT_UNKNOWN, 0);

    ConstructRenderTargetView();

    const float gameAspect = static_cast<float>(gameSize_.x) / static_cast<float>(gameSize_.y);
    const float winAspect  = static_cast<float>(newWidth) / static_cast<float>(newHeight);

    if (winAspect > gameAspect)
    {
        viewport_.Height   = static_cast<float>(newHeight);
        viewport_.Width    = viewport_.Height * gameAspect;
        viewport_.TopLeftX = (static_cast<float>(newWidth) - viewport_.Width) / 2.0f;
        viewport_.TopLeftY = 0.0f;
    }
    else
    {
        viewport_.Width    = static_cast<float>(newWidth);
        viewport_.Height   = viewport_.Width / gameAspect;
        viewport_.TopLeftX = 0.0f;
        viewport_.TopLeftY = (static_cast<float>(newHeight) - viewport_.Height) / 2.0f;
    }
}

void Pipeline::Destroy() const
{
    pBlendState_->Release();
    pRenderTargetView_->Release();
    pSwapChain_->Release();
    pDeviceContext_->Release();
    pDevice_->Release();
}

void Pipeline::Add(Renderer* pRenderAble)
{
    renderAbles_.push_back(pRenderAble);
    pRenderAble->Construct(this);
}

void Pipeline::ConstructDeviceAndSwapChain(PHandlerWindow pHandlerWindow)
{
    constexpr D3D_FEATURE_LEVEL featureLevel[] = {D3D_FEATURE_LEVEL_11_1};

    DXGI_SWAP_CHAIN_DESC swapChainDescriptor;
    swapChainDescriptor.BufferCount = 2;
    swapChainDescriptor.BufferDesc.Width = size_.x;
    swapChainDescriptor.BufferDesc.Height = size_.y;
    swapChainDescriptor.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDescriptor.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDescriptor.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDescriptor.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDescriptor.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDescriptor.OutputWindow = pHandlerWindow;
    swapChainDescriptor.Windowed = true;
    swapChainDescriptor.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDescriptor.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    swapChainDescriptor.SampleDesc.Count = 1;
    swapChainDescriptor.SampleDesc.Quality = 0;

    D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        featureLevel, 1, D3D11_SDK_VERSION, &swapChainDescriptor,
        &pSwapChain_, &pDevice_, nullptr, &pDeviceContext_);
}

void Pipeline::ConstructBlendState()
{
    D3D11_BLEND_DESC blendDescriptor = {};
    blendDescriptor.RenderTarget[0].BlendEnable           = TRUE;
    blendDescriptor.RenderTarget[0].SrcBlend              = D3D11_BLEND_SRC_ALPHA;
    blendDescriptor.RenderTarget[0].DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
    blendDescriptor.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
    blendDescriptor.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
    blendDescriptor.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
    blendDescriptor.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
    blendDescriptor.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    pDevice_->CreateBlendState(&blendDescriptor, &pBlendState_);
}

void Pipeline::ConstructRenderTargetView()
{
    ID3D11Texture2D* backgroundTexture;
    pSwapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backgroundTexture));
    pDevice_->CreateRenderTargetView(backgroundTexture, nullptr, &pRenderTargetView_);
    backgroundTexture->Release();
}
