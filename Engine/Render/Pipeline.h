#pragma once
#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include "Able.h"
#include "../Lib/Types.h"

namespace Engine
{
    namespace Render
    {
        class Able;

        class Pipeline final
        {
        public:
            void Compose(PHandlerWindow pHandlerWindow, const Point& size);
            void Render(float delta) const;
            void Resize(int newWidth, int newHeight);
            void Destroy() const;
            void Add(Able* pRenderAble);

            DXDevice* GetDevice() const { return pDevice_.Get(); }
            DXDeviceContext* GetDeviceContext() const { return pDeviceContext_; }
            PHandlerWindow GetWindow() const { return hwnd_; }

        private:
            void ComposeDeviceAndSwapChain(PHandlerWindow pHandlerWindow);
            void ComposeRenderTargetView();
            void ComposeBlendState();

            Point size_{};
            Point gameSize_{};
            DXViewport viewport_{};
            Microsoft::WRL::ComPtr<DXDevice> pDevice_;
            DXDeviceContext* pDeviceContext_ = nullptr;
            DXSwapChain* pSwapChain_ = nullptr;
            DXRenderTargetView* pRenderTargetView_ = nullptr;
            DXBlendState* pBlendState_ = nullptr;
            std::vector<Able*> renderAbles_{};
            PHandlerWindow hwnd_ = nullptr;
        };
    }
}