#pragma once

#include <vector>
#include "../../Lib/Types.h"

namespace Engine::Render { class Pipeline; }

namespace Basic::Components
{
    class Rendering3D
    {
    public:
        enum class ShaderType { SolidColor, PerlinNoise, ShaderTex, Phong };

        struct LightData
        {
            float3 lightPos;
            float  _pad0       = 0.f;
            float3 lightColor  = { 1.f, 1.f, 1.f };
            float  _pad1       = 0.f;
            float3 cameraPos;
            float  _pad2       = 0.f;
            float  ambientStrength  = 0.1f;
            float  specularStrength = 0.5f;
            float  shininess        = 32.f;
            float  _pad3       = 0.f;
        };

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

        void Construct(Engine::Render::Pipeline* pPipeline,
                       const std::vector<Vertex3D>& vertices,
                       const std::vector<int32>& indices,
                       ShaderType shaderType = ShaderType::SolidColor,
                       ID3D11ShaderResourceView* pTextureSRV = nullptr);

        void DrawInstanced(const std::vector<ObjectData>& instances);

        static void SetLight(Engine::Render::Pipeline* pPipeline, const LightData& data);

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
        ShaderType                 shaderType_        = ShaderType::SolidColor;
        int32                      indexCount_        = 0;
        int32                      instanceCapacity_  = 0;
    };
}
