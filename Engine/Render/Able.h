#pragma once

namespace Engine
{
    namespace Render
    {
        class Pipeline;

        class Able
        {
        public:
            virtual ~Able() = default;

            virtual void Compose(Pipeline* pPipeline) {}
            virtual void Render(float delta) {}
        };
    }
}
