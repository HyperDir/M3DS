#pragma once

#include "CollisionObject2D.hpp"

namespace SPhys {
    class PhysicsBody2D : public CollisionObject2D {
    public:
        explicit constexpr PhysicsBody2D(ObjectType2D objectType) noexcept;
    };
}



/* Implementation */
namespace SPhys {
    constexpr PhysicsBody2D::PhysicsBody2D(const ObjectType2D objectType) noexcept : CollisionObject2D(objectType) {}
}
