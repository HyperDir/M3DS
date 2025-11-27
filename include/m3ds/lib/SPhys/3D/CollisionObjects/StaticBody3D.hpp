#pragma once

#include "PhysicsBody3D.hpp"

namespace SPhys {
    class StaticBody3D : public PhysicsBody3D {
    public:
        explicit constexpr StaticBody3D(
            const Metres<Vector3>& translation = {},
            const Quaternion& rotation = {},
            const Shape3D& shape = {}
        ) noexcept;
    };

    constexpr StaticBody3D::StaticBody3D(
        const Metres<Vector3>& translation,
        const Quaternion& rotation,
        const Shape3D& shape
    ) noexcept
        : PhysicsBody3D(translation, rotation, shape)
    {
        mObjectType = ObjectType3D::static_body;
    }
}
