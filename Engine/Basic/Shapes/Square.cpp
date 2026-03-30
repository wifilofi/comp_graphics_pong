#include "Square.h"

#include <d3dcompiler.h>
#include "../../Render/Pipeline.h"
#include <SimpleMath.h>
#pragma comment(lib, "d3dcompiler.lib")

using namespace Basic::Shapes;

void Square::Construct(const float2& center, const float2& size)
{
    pointsSquare_[0] = float4(center.x + size.x, center.y + size.y, 0.5f, 1.0f);
    pointsSquare_[1] = float4(1.0f, 0.0f, 0.0f, 0.0f); // uv (1, 0) top-right
    pointsSquare_[2] = float4(center.x - size.x, center.y - size.y, 0.5f, 1.0f);
    pointsSquare_[3] = float4(0.0f, 1.0f, 0.0f, 0.0f); // uv (0, 1) bottom-left
    pointsSquare_[4] = float4(center.x + size.x, center.y - size.y, 0.5f, 1.0f);
    pointsSquare_[5] = float4(1.0f, 1.0f, 0.0f, 0.0f); // uv (1, 1) bottom-right
    pointsSquare_[6] = float4(center.x - size.x, center.y + size.y, 0.5f, 1.0f);
    pointsSquare_[7] = float4(0.0f, 0.0f, 0.0f, 0.0f); // uv (0, 0) top-left
    pPoints_ = pointsSquare_;

    indicesSquare_[0] = 0;
    indicesSquare_[1] = 1;
    indicesSquare_[2] = 2;
    indicesSquare_[3] = 1;
    indicesSquare_[4] = 0;
    indicesSquare_[5] = 3;
    pIndices_ = indicesSquare_;

    indicesAmount_ = 6;
}

void Square::Construct(Engine::Render::Pipeline* pPipeline)
{
    Rendering::Construct(pPipeline);
//
    CreateVertexShader(L"././Shaders/SDFSmooth.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE);
    CreatePixelShader(L"././Shaders/SDFSmooth.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE);
    CreateLayout();
    CreateVertexBuffer(4 * 2);
    CreateIndexBuffer(3 * 2);
    CreateRasterizerState();

    pPipeline_->GetDeviceContext()->RSSetState(pRasterizerState_);
}
