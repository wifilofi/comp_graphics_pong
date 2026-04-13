#pragma once

#include <vector>
#include "../../Lib/Types.h"

namespace Engine::Render { class Pipeline; }

namespace Basic::Components
{
    class Rendering3D
    {
    public:
        enum class ShaderType { SolidColor, PerlinNoise };

        struct Vertex3D
        {
            float3 position;
            float3 normal;
        };

        struct ObjectData
        {
            float4x4 model;
            float4   color;
            float4   color2;  // used by PerlinNoise shader; ignored by SolidColor
        };

        void Construct(Engine::Render::Pipeline* pPipeline,
                       const std::vector<Vertex3D>& vertices,
                       const std::vector<int32>& indices,
                       ShaderType shaderType = ShaderType::SolidColor);

        void Draw(const float4x4& model, const float4& color,
                  const float4& color2 = float4(1, 1, 1, 1));

    private:
        Engine::Render::Pipeline* pPipeline_ = nullptr;
        DXVertexShader*    pVertexShader_  = nullptr;
        DXPixelShader*     pPixelShader_   = nullptr;
        DXInputLayout*     pInputLayout_   = nullptr;
        DXBuffer*          pVertexBuffer_  = nullptr;
        DXBuffer*          pIndexBuffer_   = nullptr;
        DXBuffer*          pObjectBuffer_  = nullptr;
        DXRasterizerState* pRasterizerState_ = nullptr;
        int32              indexCount_     = 0;
    };
}
