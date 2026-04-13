#pragma once

namespace Engine
{
    namespace Update
    {
        class Fixed;

        class FixedAble
        {
        public:
            virtual ~FixedAble() = default;

            virtual void Construct(Fixed* pFixed)
            {
            }

            virtual void FixedUpdate()
            {
            }
        };
    }
}
