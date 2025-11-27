#pragma once

#include "CollisionObject3D.hpp"

namespace SPhys {
    class PhysicsBody3D : public CollisionObject3D {
    public:
        explicit constexpr PhysicsBody3D(
            const Metres<Vector3>& translation,
            const Quaternion& rotation,
            const Shape3D& shape
        ) noexcept;
    };

    constexpr PhysicsBody3D::PhysicsBody3D(const Metres<Vector3>& translation, const Quaternion& rotation, const Shape3D& shape) noexcept
        : CollisionObject3D(translation, rotation, shape)
    {}
}
