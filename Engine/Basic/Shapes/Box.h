#pragma once

#include <vector>
#include "../Components/Rendering3D.h"

namespace Basic::Shapes
{
    class Box
    {
    public:
        static std::vector<Components::Rendering3D::Vertex3D> Vertices();
        static std::vector<int32> Indices();
    };
}
