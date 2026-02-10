#pragma once

#include <m3ds/spatial/Vector2.hpp>
#include <m3ds/spatial/Vector3.hpp>
#include <m3ds/spatial/Matrix4x4.hpp>

#include <m3ds/utils/LinearAllocator.hpp>

namespace M3DS {
    struct Vertex2D {
        Vector3 position {};
        Vector2 texcoord {};
        Colour colour {};
    };

    using Triangle2D = std::array<Vertex2D, 3>;
    using Quad2D = std::array<Vertex2D, 4>;

    struct Mesh2D {
        Texture texture;
        LinearHeapArray<Vertex2D> vertices {};
        Matrix4x4 transform = Matrix4x4::identity();
        Colour tint = Colours::white;
    };
}
