#pragma once

namespace Engine
{
    namespace Render
    {
        class Pipeline;

        class Renderer
        {
        public:
            virtual ~Renderer() = default;

            virtual void Construct(Pipeline* pPipeline) {}
            virtual void Render(float delta) {}
        };
    }
}
