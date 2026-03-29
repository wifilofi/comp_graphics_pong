#pragma once

#include "../../Engine/Basic/Shapes/Square.h"
#include "../../Engine/Physics/CollideAble.h"
#include "../../Engine/Physics/MoveAble.h"
#include "../../Engine/Update/FixedAble.h"
#include "../../Engine/Render/Pipeline.h"
#include "../../Engine/Lib/Types.h"

namespace Pong
{
    enum struct Side : uint8
    {
        None,
        Left,
        Right
    };

    class Stick : public Engine::Render::Able, public Engine::Update::FixedAble,
                  public Engine::Physics::CollideAble, public Engine::Physics::MoveAble
    {
    public:
        void Compose(const float2& center, const float2& size, Side side, float speed);
        void Compose(Engine::Render::Pipeline* pPipeline) override;
        void Render(float delta) override;
        DXBox& GetBoundingBox() override { return boundingBox_; }
        float2& GetVelocity() override { return velocity_; }

    protected:
        static float Clamp(float x, float upper, float down);

        Basic::Shapes::Square sprite_;
        DXBox boundingBox_;
        float2 velocity_;
        DXBuffer* pAdditionDataBuffer_ = nullptr;
        Side hand_ = Side::None;
        float speed_ = 0;
        float3 startPosition_;
    };
}
