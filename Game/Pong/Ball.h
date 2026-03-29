#pragma once

#include "../../Engine/Basic/Shapes/Square.h"
#include "../../Engine/Lib/Interpolated.h"
#include "../../Engine/Update/FixedAble.h"
#include "../../Engine/Render/Pipeline.h"
#include "../../Engine/Physics/CollideAble.h"
#include "../../Engine/Physics/MoveAble.h"

namespace Pong
{
    class Ball final : public Engine::Render::Able, public Engine::Update::FixedAble,
                       public Engine::Physics::CollideAble, public Engine::Physics::MoveAble
    {
    public:
        void Compose(const float2 &center, const float2 &size, float startSpeed, float speedIncrease);

        void Compose(Engine::Render::Pipeline *pPipeline) override;

        void FixedUpdate() override;

        void Render(float delta) override;

        void Collided(Engine::Physics::CollideAble *other) override;

        DXBox &GetBoundingBox() override { return boundingBox_; }
        float2 &GetVelocity() override { return velocity_; }

    private:
        static float Clamp(float x, float upper, float down);

        Basic::Shapes::Square sprite_;
        DXBox boundingBox_;
        float2 velocity_;
        DXBuffer *pAdditionDataBuffer_ = nullptr;
        float3 startPosition_;
        Lib::Interpolated<float3> position_;
        float startSpeed_ = 0;
        float speedIncrease_ = 0;
    };
}
