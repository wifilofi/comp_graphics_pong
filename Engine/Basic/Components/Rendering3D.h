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
            float4   color2;
        };

        void Construct(Engine::Render::Pipeline* pPipeline,
                       const std::vector<Vertex3D>& vertices,
                       const std::vector<int32>& indices,
                       ShaderType shaderType = ShaderType::SolidColor);

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
        int32                      indexCount_        = 0;
        int32                      instanceCapacity_  = 0;
    };
}
