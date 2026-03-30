#include "LowPolySphere.h"

#define _USE_MATH_DEFINES
#include <math.h>

using namespace Basic::Shapes;
using Vertex3D = Basic::Components::Rendering3D::Vertex3D;

std::vector<Vertex3D> LowPolySphere::Vertices(int slices, int stacks)
{
    std::vector<Vertex3D> verts;

    verts.push_back({{0, 1, 0}, {0, 1, 0}});

    for (int i = 1; i < stacks; ++i)
    {
        const float phi = static_cast<float>(M_PI) * i / stacks;
        for (int j = 0; j <= slices; ++j)
        {
            const float theta = 2.f * static_cast<float>(M_PI) * j / slices;
            const float x = sinf(phi) * cosf(theta);
            const float y = cosf(phi);
            const float z = sinf(phi) * sinf(theta);
            verts.push_back({{x, y, z}, {x, y, z}});
        }
    }

    verts.push_back({{0, -1, 0}, {0, -1, 0}});

    return verts;
}

std::vector<int32> LowPolySphere::Indices(int slices, int stacks)
{
    std::vector<int32> idx;

    for (int j = 0; j < slices; ++j)
        idx.insert(idx.end(), {0, 1 + j, 1 + j + 1});

    for (int i = 1; i < stacks - 1; ++i)
    {
        const int32 row = 1 + (i - 1) * (slices + 1);
        const int32 next = 1 + i * (slices + 1);
        for (int j = 0; j < slices; ++j)
        {
            idx.insert(idx.end(), {row + j, next + j, next + j + 1});
            idx.insert(idx.end(), {row + j, next + j + 1, row + j + 1});
        }
    }

    const int32 lastVert = 1 + (stacks - 1) * (slices + 1);
    const int32 lastRing = 1 + (stacks - 2) * (slices + 1);
    for (int j = 0; j < slices; ++j)
        idx.insert(idx.end(), {lastVert, lastRing + j + 1, lastRing + j});

    return idx;
}
