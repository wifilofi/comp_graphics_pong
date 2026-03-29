#pragma once

#include "../Lib/Types.h"

namespace Engine
{
    namespace Input
    {
        namespace Mouse
        {
            enum struct Buttons : uint16
            {
                None = 0,
                LeftButtonDown = 1,
                LeftButtonUp = 1 << 1,
                RightButtonDown = 1 << 2,
                RightButtonUp = 1 << 3,
                WheelButtonDown = 1 << 4,
                WheelButtonUp = 1 << 5
            };

            struct Event
            {
                Point Position;
                Point Translation;
                uint16 WheelDelta;

                Event() : Position(), Translation(), WheelDelta(0)
                {
                }

                Event(const Point& position, const Point& translation, uint16 wheelDelta)
                    : Position(position), Translation(translation), WheelDelta(wheelDelta)
                {
                }
            };
        }
    }
}
