#include "Box.h"

using namespace Basic::Shapes;
using Vertex3D = Basic::Components::Rendering3D::Vertex3D;

// Unit cube [-0.5, 0.5] on each axis, 24 vertices (4 per face for flat normals)
std::vector<Vertex3D> Box::Vertices()
{
    return {
        // +X face (right)
        {{0.5f,-0.5f,-0.5f},{1,0,0}}, {{0.5f, 0.5f,-0.5f},{1,0,0}},
        {{0.5f, 0.5f, 0.5f},{1,0,0}}, {{0.5f,-0.5f, 0.5f},{1,0,0}},
        // -X face (left)
        {{-0.5f,-0.5f, 0.5f},{-1,0,0}}, {{-0.5f, 0.5f, 0.5f},{-1,0,0}},
        {{-0.5f, 0.5f,-0.5f},{-1,0,0}}, {{-0.5f,-0.5f,-0.5f},{-1,0,0}},
        // +Y face (top)
        {{-0.5f,0.5f,-0.5f},{0,1,0}}, {{-0.5f,0.5f, 0.5f},{0,1,0}},
        {{ 0.5f,0.5f, 0.5f},{0,1,0}}, {{ 0.5f,0.5f,-0.5f},{0,1,0}},
        // -Y face (bottom)
        {{-0.5f,-0.5f, 0.5f},{0,-1,0}}, {{-0.5f,-0.5f,-0.5f},{0,-1,0}},
        {{ 0.5f,-0.5f,-0.5f},{0,-1,0}}, {{ 0.5f,-0.5f, 0.5f},{0,-1,0}},
        // +Z face (front)
        {{-0.5f,-0.5f,0.5f},{0,0,1}}, {{ 0.5f,-0.5f,0.5f},{0,0,1}},
        {{ 0.5f, 0.5f,0.5f},{0,0,1}}, {{-0.5f, 0.5f,0.5f},{0,0,1}},
        // -Z face (back)
        {{ 0.5f,-0.5f,-0.5f},{0,0,-1}}, {{-0.5f,-0.5f,-0.5f},{0,0,-1}},
        {{-0.5f, 0.5f,-0.5f},{0,0,-1}}, {{ 0.5f, 0.5f,-0.5f},{0,0,-1}},
    };
}

std::vector<int32> Box::Indices()
{
    std::vector<int32> idx;
    idx.reserve(36);
    for (int32 f = 0; f < 6; ++f)
    {
        int32 b = f * 4;
        idx.insert(idx.end(), {b,b+1,b+2, b,b+2,b+3});
    }
    return idx;
}
