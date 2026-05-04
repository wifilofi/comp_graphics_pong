#pragma once

#include <vector>
#include "../../Lib/Types.h"

namespace Engine::Render { class Pipeline; }

namespace Basic::Components
{
    class Rendering3D
    {
    public:
        enum class ShaderType { SolidColor, PerlinNoise, ShaderTex };

        struct Vertex3D
        {
            float3 position;
            float3 normal;
            float2 uv = {};
        };

        struct ObjectData
        {
            float4x4 model;
            float4   color;
            float4   color2;
        };

        struct LightData
        {
            float3 lightDir;      // world-space direction the light points toward
            float  _pad0 = 0.f;
            float3 lightColor;
            float  _pad1 = 0.f;
            float3 ambientColor;
            float  _pad2 = 0.f;
        };

        void Construct(Engine::Render::Pipeline* pPipeline,
                       const std::vector<Vertex3D>& vertices,
                       const std::vector<int32>& indices,
                       ShaderType shaderType = ShaderType::SolidColor,
                       ID3D11ShaderResourceView* pTextureSRV = nullptr);

        void SetLight(const LightData& light) { light_ = light; }
        void DrawInstanced(const std::vector<ObjectData>& instances);

    private:
        void EnsureInstanceBuffer(int count);

        Engine::Render::Pipeline*  pPipeline_        = nullptr;
        DXVertexShader*            pVertexShader_     = nullptr;
        DXPixelShader*             pPixelShader_      = nullptr;
        DXInputLayout*             pInputLayout_      = nullptr;
        DXBuffer*                  pVertexBuffer_     = nullptr;
        DXBuffer*                  pIndexBuffer_      = nullptr;
        DXBuffer*                  pInstanceBuffer_   = nullptr;
        ID3D11ShaderResourceView*  pInstanceSRV_      = nullptr;
        DXRasterizerState*         pRasterizerState_  = nullptr;
        ID3D11ShaderResourceView*  pTextureSRV_       = nullptr;
        ID3D11SamplerState*        pSamplerState_     = nullptr;
        DXBuffer*                  pLightBuffer_      = nullptr;
        LightData                  light_             = { {0.4f,-1.f,0.6f}, 0.f, {1.f,1.f,1.f}, 0.f, {0.15f,0.15f,0.15f}, 0.f };
        ShaderType                 shaderType_        = ShaderType::SolidColor;
        int32                      indexCount_        = 0;
        int32                      instanceCapacity_  = 0;
    };
}
