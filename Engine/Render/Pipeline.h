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
        class Camera;

        class Pipeline final
        {
        public:
            void Construct(PHandlerWindow pHandlerWindow, const Point& size);
            void Render(float delta) const;
            void Resize(int newWidth, int newHeight);
            void Destroy() const;
            void Add(Renderer* pRenderAble);
            void SetCamera(Camera* pCamera);

            DXDevice* GetDevice() const { return pDevice_.Get(); }
            DXDeviceContext* GetDeviceContext() const { return pDeviceContext_; }
            PHandlerWindow GetWindow() const { return hwnd_; }
            const DXViewport& GetViewport() const { return viewport_; }

            struct FrameData
            {
                float  time;
                float  _pad;
                float2 resolution;
            };

            struct CameraData
            {
                float4x4 view;
                float4x4 projection;
            };

        private:
            void ConstructDeviceAndSwapChain(PHandlerWindow pHandlerWindow);
            void ConstructRenderTargetView();
            void ConstructBlendState();
            void ConstructDepthBuffer(int width, int height);

            Point size_{};
            Point gameSize_{};
            DXViewport viewport_{};
            Microsoft::WRL::ComPtr<DXDevice> pDevice_;
            DXDeviceContext* pDeviceContext_ = nullptr;
            DXSwapChain* pSwapChain_ = nullptr;
            DXRenderTargetView* pRenderTargetView_ = nullptr;
            DXDepthStencilView* pDepthStencilView_ = nullptr;
            DXBlendState* pBlendState_ = nullptr;
            DXBuffer* pFrameDataBuffer_ = nullptr;
            DXBuffer* pCameraBuffer_ = nullptr;
            mutable float time_ = 0.f;
            std::vector<Renderer*> renderAbles_{};
            PHandlerWindow hwnd_ = nullptr;
            Camera* pCamera_ = nullptr;
        };
    }
}