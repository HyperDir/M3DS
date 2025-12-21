#pragma once

#include "PhysicsBody2D.hpp"

namespace SPhys {
    class StaticBody2D : public PhysicsBody2D {
    public:
        constexpr StaticBody2D() noexcept;
    };

    constexpr StaticBody2D::StaticBody2D() noexcept : PhysicsBody2D(ObjectType2D::static_body) {}
}
