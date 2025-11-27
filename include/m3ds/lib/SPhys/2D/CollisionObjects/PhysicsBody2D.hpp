#pragma once

#include "CollisionObject2D.hpp"

namespace SPhys {
    class PhysicsBody2D : public CollisionObject2D {
    public:
        explicit constexpr PhysicsBody2D(
            const Pixels<Vector2>& translation,
            float rotation,
            const Shape2D& shape
        ) noexcept;
    };

    constexpr PhysicsBody2D::PhysicsBody2D(const Pixels<Vector2>& translation, const float rotation, const Shape2D& shape) noexcept
        : CollisionObject2D(translation, rotation, shape)
    {}
}
