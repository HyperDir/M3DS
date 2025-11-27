#pragma once

#include "PhysicsBody2D.hpp"

namespace SPhys {
    class StaticBody2D : public PhysicsBody2D {
    public:
        explicit constexpr StaticBody2D(
            const Pixels<Vector2>& translation = {},
            float rotation = {},
            const Shape2D& shape = {}
        ) noexcept;
    };

    constexpr StaticBody2D::StaticBody2D(
        const Pixels<Vector2>& translation,
        const float rotation,
        const Shape2D& shape
    ) noexcept
        : PhysicsBody2D(translation, rotation, shape)
    {
        mObjectType = ObjectType2D::static_body;
    }
}
