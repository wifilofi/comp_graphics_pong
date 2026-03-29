#pragma once

#include <vector>

#include "MoveAble.h"
#include "../Update/FixedAble.h"
#include "../Update/Fixed.h"

namespace Engine
{
    namespace Physics
    {
        class MoveAble;

        class Move final : public Update::FixedAble
        {
        public:
            void Compose(Update::Fixed* pFixed) override;
            void FixedUpdate() override;

            void Add(MoveAble* pMoveAble);

        private:
            std::vector<MoveAble*> moveAbles_{};
            Update::Fixed* pFixed_ = nullptr;
        };
    }
}
