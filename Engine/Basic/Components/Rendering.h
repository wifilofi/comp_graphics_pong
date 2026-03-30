#pragma once

#include "../../Lib/Types.h"
#include "../../Render/Renderer.h"

namespace Engine::Render
{
    class Pipeline;
}

namespace Basic
{
    namespace Components
    {
        class Rendering : public Engine::Render::Renderer
        {
        public:
            void Construct(Engine::Render::Pipeline *pPipeline) override;

            void Render(float delta) override;

            Engine::Render::Pipeline *GetPipeline() const { return pPipeline_; }

        protected:
            void CreateVertexShader(const char16 *pPath, const DXShaderMacros *pDefines, DXIncludes *pIncludes);

            void CreatePixelShader(const char16 *pPath, const DXShaderMacros *pDefines, DXIncludes *pIncludes);

            void CreateLayout();

            void CreateVertexBuffer(uint32 pointsAmount);

            void CreateIndexBuffer(uint32 indicesAmount);

            void CreateRasterizerState();

            Engine::Render::Pipeline *pPipeline_ = nullptr;
            DXBlob *pVertexShaderByteCode_ = nullptr;
            DXBlob *pPixelShaderByteCode_ = nullptr;
            DXVertexShader *pVertexShader_ = nullptr;
            DXPixelShader *pPixelShader_ = nullptr;
            DXInputLayout *pInputLayout_ = nullptr;
            float4 *pPoints_ = nullptr;
            int32 *pIndices_ = nullptr;
            DXBuffer *pVertexBuffer_ = nullptr;
            DXBuffer *pIndexBuffer_ = nullptr;
            DXRasterizerState *pRasterizerState_ = nullptr;
            int32 indicesAmount_ = 0;
        };
    }
}
