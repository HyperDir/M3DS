#pragma once

#include "PhysicsBody3D.hpp"

namespace SPhys {
    class StaticBody3D : public PhysicsBody3D {
    public:
        constexpr StaticBody3D() noexcept;
    };

    constexpr StaticBody3D::StaticBody3D() noexcept : PhysicsBody3D(ObjectType3D::static_body) {}
}
