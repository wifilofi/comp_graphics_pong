#pragma once

#include <vector>
#include "../Components/Rendering3D.h"

namespace Basic::Shapes
{
    class LowPolySphere
    {
    public:
        static std::vector<Components::Rendering3D::Vertex3D> Vertices(int slices = 8, int stacks = 5);
        static std::vector<int32> Indices(int slices = 8, int stacks = 5);
    };
}
