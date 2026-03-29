#include "Pipeline.h"

#pragma comment(lib, "d3d11.lib")

using namespace Engine::Render;

void Pipeline::Compose(PHandlerWindow pHandlerWindow, const Point& size)
{
    hwnd_ = pHandlerWindow;
    size_ = size;
    viewport_.Width = static_cast<float>(size.x);
    viewport_.Height = static_cast<float>(size.y);
    viewport_.TopLeftX = 0;
    viewport_.TopLeftY = 0;
    viewport_.MinDepth = 0;
    viewport_.MaxDepth = 1.0f;
    ComposeDeviceAndSwapChain(pHandlerWindow);
    ComposeRenderTargetView();
}
void Pipeline::Render(float delta) const
{
    pDeviceContext_->ClearState();
    pDeviceContext_->OMSetRenderTargets(1, &pRenderTargetView_, nullptr);
    constexpr float color[] = {0.0f, 0.0f, 0.0f, 1.0f};
    pDeviceContext_->ClearRenderTargetView(pRenderTargetView_, color);
    pDeviceContext_->RSSetViewports(1, &viewport_);
    for (auto* pRenderAble : renderAbles_)
    {
        pDeviceContext_->OMSetRenderTargets(1, &pRenderTargetView_, nullptr);
        pRenderAble->Render(delta);
    }
    pSwapChain_->Present(1, DXGI_PRESENT_DO_NOT_WAIT);
}

void Pipeline::Destroy() const
{
    pRenderTargetView_->Release();
    pSwapChain_->Release();
    pDeviceContext_->Release();
    pDevice_->Release();
}

void Pipeline::Add(Able* pRenderAble)
{
    renderAbles_.push_back(pRenderAble);
    pRenderAble->Compose(this);
}

void Pipeline::ComposeDeviceAndSwapChain(PHandlerWindow pHandlerWindow)
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

void Pipeline::ComposeRenderTargetView()
{
    ID3D11Texture2D* backgroundTexture;
    pSwapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backgroundTexture));
    pDevice_->CreateRenderTargetView(backgroundTexture, nullptr, &pRenderTargetView_);
}
