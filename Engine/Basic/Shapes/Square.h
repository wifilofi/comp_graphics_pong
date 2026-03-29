#pragma once

#include "../Components/Rendering.h"

namespace Basic
{
    namespace Shapes
    {
        class Square : public Components::Rendering
        {
        public:
            void Compose(const float2 &center, const float2 &size);

            void Compose(Engine::Render::Pipeline *pPipeline) override;

        protected:
            float4 pointsSquare_[4 * 2]{};
            int32 indicesSquare_[3 * 2]{};
        };
    }
}
