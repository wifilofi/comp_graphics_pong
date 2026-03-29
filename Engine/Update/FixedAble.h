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

            virtual void Compose(Fixed* pFixed)
            {
            }

            virtual void FixedUpdate()
            {
            }
        };
    }
}
