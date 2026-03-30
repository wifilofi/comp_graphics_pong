#pragma once

#include <vector>
#include "../../Lib/Types.h"

namespace Engine::Render { class Pipeline; }

namespace Basic::Components
{
    class Rendering3D
    {
    public:
        struct Vertex3D
        {
            float3 position;
            float3 normal;
        };

        struct ObjectData
        {
            float4x4 model;
            float4   color;
        };

        void Construct(Engine::Render::Pipeline* pPipeline,
                       const std::vector<Vertex3D>& vertices,
                       const std::vector<int32>& indices);

        void Draw(const float4x4& model, const float4& color);

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
