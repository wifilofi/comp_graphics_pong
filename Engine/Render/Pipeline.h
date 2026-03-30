#pragma once
#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include "Renderer.h"
#include "../Lib/Types.h"

namespace Engine
{
    namespace Render
    {
        class Renderer;

        class Pipeline final
        {
        public:
            void Construct(PHandlerWindow pHandlerWindow, const Point& size);
            void Render(float delta) const;
            void Resize(int newWidth, int newHeight);
            void Destroy() const;
            void Add(Renderer* pRenderAble);

            DXDevice* GetDevice() const { return pDevice_.Get(); }
            DXDeviceContext* GetDeviceContext() const { return pDeviceContext_; }
            PHandlerWindow GetWindow() const { return hwnd_; }
            const DXViewport& GetViewport() const { return viewport_; }

        private:
            void ConstructDeviceAndSwapChain(PHandlerWindow pHandlerWindow);
            void ConstructRenderTargetView();
            void ConstructBlendState();

            Point size_{};
            Point gameSize_{};
            DXViewport viewport_{};
            Microsoft::WRL::ComPtr<DXDevice> pDevice_;
            DXDeviceContext* pDeviceContext_ = nullptr;
            DXSwapChain* pSwapChain_ = nullptr;
            DXRenderTargetView* pRenderTargetView_ = nullptr;
            DXBlendState* pBlendState_ = nullptr;
            std::vector<Renderer*> renderAbles_{};
            PHandlerWindow hwnd_ = nullptr;
        };
    }
}