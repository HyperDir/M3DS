#pragma once

#include "CollisionObject3D.hpp"

namespace SPhys {
    class PhysicsBody3D : public CollisionObject3D {
    public:
        explicit constexpr PhysicsBody3D(ObjectType3D objectType) noexcept;
    };

    constexpr PhysicsBody3D::PhysicsBody3D(const ObjectType3D objectType) noexcept : CollisionObject3D(objectType) {}
}
