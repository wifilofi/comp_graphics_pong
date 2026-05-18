#pragma once
#include <vector>
#include <d3d11.h>
#include "../Lib/Types.h"

namespace Engine::Render { class Pipeline; }

namespace Engine::Render
{
    class PostProcess
    {
    public:
        void Construct(Pipeline* pPipeline, int width, int height);
        void AddPass(const wchar_t* psFile);
        void Resize(int width, int height);

        ID3D11RenderTargetView* GetSceneRTV() const { return pSceneRTV_[0]; }
        ID3D11DepthStencilView* GetSceneDSV() const { return pSceneDSV_; }

        // Pipeline calls this after scene is rendered
        void Apply(ID3D11RenderTargetView* pBackBuffer);

    private:
        void CreateTextures(int w, int h);
        void DestroyTextures();
        void EnsureInfra();

        Pipeline*                 pPipeline_    = nullptr;
        ID3D11Texture2D*          pTex_[2]      = {};
        ID3D11RenderTargetView*   pSceneRTV_[2] = {};
        ID3D11ShaderResourceView* pSceneSRV_[2] = {};
        ID3D11Texture2D*          pDepthTex_    = nullptr;
        ID3D11DepthStencilView*   pSceneDSV_    = nullptr;
        ID3D11VertexShader*       pFullscreenVS_ = nullptr;
        ID3D11SamplerState*       pSampler_      = nullptr;
        ID3D11DepthStencilState*  pNoDepthState_ = nullptr;
        int                       width_         = 0;
        int                       height_        = 0;

        struct Pass { ID3D11PixelShader* pPS = nullptr; };
        std::vector<Pass> passes_;
    };
}
