#pragma once

#include <vector>
#include "../Components/Rendering3D.h"

namespace Basic::Shapes
{
    class LowPolySphere
    {
    public:
        static std::vector<Components::Rendering3D::Vertex3D> Vertices(int slices = 24, int stacks = 16);
        static std::vector<int32> Indices(int slices = 24, int stacks = 16);
    };
}
